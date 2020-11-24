/**
 ** This file is part of the Stuff project.
 ** Copyright 2020 darksider3 <github@darksider3.de>.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include <cstring> // memset...
#include <deque>
#include <functional>
#include <locale.h>
#include <ncurses.h>
#include <signal.h>

#include "LiGi/ThreadsafePrimitive.h"
#include "LiGi/TimerTools.h"
#include "LiGi/stack.h"
#include "LiGi/timer.h"

#include "ApplicationDefaults.h"
#include "SingleThreadLoop.h"
#include "Tools.h"

//@TODO: In case <semaphore> ever get's released, use it for the signal handler FFS!
struct TimerStates {
	std::atomic_bool _INTERRUPTED_ = false; // Signal Interrupt handle
	WINDOW* FullWin;                        // full window; used to get dimensions
	WINDOW* TopPanel;                       // Subwindow in FullWin, used for the top bar
	WINDOW* MidWin;                         // also in the FullWindow, used for status and timer itself
	WINDOW* ShortcutWin;                    // and again in FullWindow, here for visibility of shortcuts
	WINDOW* StatisticsWin;
} AppState;

int FULL_COORD_X, FULL_COORD_Y;

enum PomoState {
    POMODORO,
    SHORT,
    LONG,
    PAUSE,
    STOP
};

struct PomoStatistics {
public:
    //using N_I_Type = ThreadsafePrimitive<uint64_t>;
    using N_I_Type = ThreadsafePrimitive<uint64_t>;

public:
    N_I_Type M_ShortBreaks { 0 };
    N_I_Type M_LongBreaks { 0 };
    N_I_Type M_Pomodoros { 0 };
    N_I_Type M_TotalWorkingTime { 0 }; // it's not deducable, because we're going to add *any* time here we've worked on pomodoros
    N_I_Type M_TotalShortBreakTime { 0 };
    N_I_Type M_TotalLongBreakTime { 0 };
    N_I_Type M_TotalPauseTime { 0 }; // Logic to stop instead of default pause;
    N_I_Type M_TotalStopTime { 0 };  // Difference to Pause is that we stopped instead of paused, @todo logic for that
};

class PomodoroTimer : public Li::Timer<PomodoroTimer, uint64_t> {
private:
    uint64_t M_TimeLeftBeforePause = 0;

    void run(Li::Literals::TimeValue auto Goal) noexcept
    {
        this->setGoal(Goal);
        this->ResetTime();
        this->RunTimer();
        return;
    }

    void IncrIfNotStopped(PomoStatistics::N_I_Type& val)
    {
        if (!getStopper())
            val++;
        return;
    }

public:
    class View {
    private:
        PomodoroTimer const& M_Timer;
        PomoStatistics& M_Stats;

    public:
        explicit View(PomodoroTimer const& timer, PomoStatistics& outer)
            : M_Timer(timer)
            , M_Stats(outer)
        {
        }

        static void set_red(WINDOW& win)
        {
            wcolor_set(&win, 2, 0);
            return;
        }

        static void set_white(WINDOW& win)
        {
            wcolor_set(&win, 1, 0);
            return;
        }

        void Mode(WINDOW& win) const noexcept
        {
            const PomoState& state = M_Timer.getState();
            size_t midy, midx;
            getmaxyx(&win, midy, midx);
            midy = midy / 2;
            if (state == PomoState::SHORT)
				mvwaddstr(&win, static_cast<int>(midy + 2), TimerApp::xMiddle(midx, 14), "Taking a break");
            else if (state == PomoState::LONG)
				mvwaddstr(&win, static_cast<int>(midy + 2), TimerApp::xMiddle(midx, 18), "Taking a big break!");
            else if (state == PomoState::POMODORO)
				mvwaddstr(&win, static_cast<int>(midy + 2), TimerApp::xMiddle(midx, 22), "Working on a Pomodoro!");
            else if (state == PomoState::PAUSE)
				mvwaddstr(&win, static_cast<int>(midy + 2), TimerApp::xMiddle(midx, 22), "Taking a manual pause!");
            else if (state == PomoState::STOP)
				mvwaddstr(&win, static_cast<int>(midy + 2), TimerApp::xMiddle(midx, 25), "Waiting for input what to run!!");
            return;
        }

        void Mid(WINDOW& win) const noexcept
        {
            size_t midx, midy;
            getmaxyx(&win, midy, midx);
            midy = midy / 2;

            set_red(win);
            switch (M_Timer.getState()) {
            case (PomoState::PAUSE):
				TimerApp::EraseSpecific(&win, static_cast<int>(midy), TimerApp::xMiddle(midx, 7));
				mvwprintw(&win, static_cast<int>(midy), TimerApp::xMiddle(midx, 6), "Paused!");
                break;
            case (PomoState::STOP):
				TimerApp::EraseSpecific(&win, static_cast<int>(midy), TimerApp::xMiddle(midx, 7));
				mvwprintw(&win, static_cast<int>(midy), TimerApp::xMiddle(midx, 7), "STOPPED!");
                break;
            case (PomoState::LONG):
            case (PomoState::SHORT):
            case (PomoState::POMODORO):
				TimerApp::EraseSpecific(&win, static_cast<int>(midy), TimerApp::xMiddle(midx, 10));
				mvwprintw(&win, static_cast<int>(midy), TimerApp::xMiddle(midx, 8),
                    Li::TimerTools::Format::getFullTimeString(
                        M_Timer.getTimeLeft())
                        .c_str());
                break;
                //default:
                //  break;
            }
            wrefresh(&win);
            set_white(win);
            return;
        }

        void Shortcut(WINDOW& win) const noexcept
        {

            std::string title = "Shortcuts";
            box(&win, 0, 0);
            int win_x;

            win_x = getmaxx(&win);

            mvwaddstr(&win, 0, static_cast<int>((win_x - (title.length() - 1)) / 2), title.c_str());
            mvwaddstr(&win, 1, 2, "-> (C)lose");
            mvwaddstr(&win, 2, 2, "-> (B)reak");
            mvwaddstr(&win, 3, 2, "-> (P)ause/Un(P)ause");
            mvwaddstr(&win, 4, 2, "-> P(O)modoro");
            mvwaddstr(&win, 5, 2, "-> (L)ong Break");
            wrefresh(&win);
            return;
        }

        void Statistcs(WINDOW& win) noexcept
        {
            std::string title = "Statistics";
            // TODO: Long "B's"
            // TODO: Times total run in those modes
            // TOOD: For the statistics themselfes, just support
            std::string pomo = "-> Pomodoros: " + std::to_string(M_Stats.M_Pomodoros.get());
            std::string sbreaks = "-> Short B's: " + std::to_string(M_Stats.M_ShortBreaks.get());
            int win_x = getmaxx(&win);

            box(&win, 0, 0);
            mvwaddstr(&win, 0, static_cast<int>((win_x - (title.length() - 1)) / 2), title.c_str());
            mvwaddstr(&win, 1, 1, pomo.c_str());
            mvwaddstr(&win, 2, 1, sbreaks.c_str());
            wrefresh(&win);
            return;
        }

        void TitleBar(WINDOW& win) const noexcept
        {
            mvwaddstr(&win, 1, 1, "(E)dit settings");
            mvwhline(&win, 2, 0, ACS_HLINE, COLS);
            wrefresh(&win);
            return;
        }
    };

    friend View;

    std::atomic<PomoState> M_state;
    std::atomic<PomoState> M_oldState;

    PomoStatistics& M_Stats;

    using Timer<PomodoroTimer>::Timer;

    explicit PomodoroTimer(std::atomic_bool& stopper, PomoStatistics& stat)
        : Li::Timer<PomodoroTimer, uint64_t>(stopper, POMODORO_TIME)
        , M_Stats(stat)
    {
        this->setDelay(50);
        this->setSleep(10);
    }

    void RunResume() noexcept
    {
        if (M_TimeLeftBeforePause == 0) {
            RunStop();
            return;
        }

        M_state.store(M_oldState);
        setTimeLeft(M_TimeLeftBeforePause);
        Resume(); // utilize parent!
        M_oldState.store(M_state);
        M_state.store(PomoState::STOP);
        return;
    }

    void RunPomo(uint64_t Goal = POMODORO_TIME) noexcept
    {
        M_state.store(PomoState::POMODORO);
        run(Goal);
        IncrIfNotStopped(M_Stats.M_Pomodoros);
        M_oldState.store(M_state);
        M_state.store(PomoState::STOP);
        return;
    }

    void RunShortBreak(uint64_t Goal = SHORT_BREAK_TIME) noexcept
    {
        M_state = PomoState::SHORT;
        run(Goal);
        IncrIfNotStopped(M_Stats.M_ShortBreaks);
        M_oldState.store(M_state);
        M_state = PomoState::STOP;
        return;
    }

    void RunBigBreak(uint64_t Goal = BIG_BREAK_TIME) noexcept
    {
        M_state = PomoState::LONG;
        run(Goal);
        IncrIfNotStopped(M_Stats.M_LongBreaks);
        M_oldState.store(M_state);
        M_state = PomoState::STOP;
        return;
    }

    void RunPause(uint64_t Goal = PAUSE_STOP_VAL) noexcept
    {
        uint64_t CurTimeLeft = this->getTimeLeft();
        M_state = PomoState::PAUSE;
        run(Goal);
        M_Stats.M_TotalPauseTime = M_Stats.M_TotalPauseTime + this->getTimeLeft();
        M_state.store(M_oldState);
        M_TimeLeftBeforePause = CurTimeLeft;
        return;
    }

    void RunStop(uint64_t Goal = PAUSE_STOP_VAL) noexcept
    {
        M_oldState.store(M_state);
        M_state = PomoState::STOP;
        run(Goal);
        return;
    }

    PomoState getState() const noexcept { return M_state.load(); }

    const std::string getTimeStr() const noexcept
    {
        namespace Format = Li::TimerTools::Format;
        Li::Literals::TimeValue auto t = this->getTimeLeft();
        return Format::getMinutes(t) + ":" + Format::getSeconds(t);
    }
};

void quitter() noexcept
{
	delwin(AppState.FullWin);
	delwin(AppState.MidWin);
	delwin(AppState.TopPanel);
	delwin(AppState.ShortcutWin);
    endwin();
}

void init_colors() noexcept
{
    if (start_color() == ERR || !has_colors() || !can_change_color()) {
        std::cerr << "Couldn't setup coloring!" << std::endl;
        quitter();
    }
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
}

void init_windows() noexcept
{
	if ((AppState.FullWin = initscr()) == nullptr) {
        std::cerr << "Error! Couldn't initialise ncurses!" << std::endl;
        quitter();
    }
    atexit(quitter);

	getmaxyx(AppState.FullWin, FULL_COORD_Y, FULL_COORD_X);

    if (FULL_COORD_Y < MIN_Y || FULL_COORD_X < MIN_X) {
        std::cerr << "Sorry, but your window isnt big enough!" << std::endl;
        exit(ERR);
    }

	if ((AppState.TopPanel = newwin(3, FULL_COORD_X, 0, 0)) == nullptr) {
        std::cerr << "Error! Couldn't intiialise top panel!" << std::endl;
        exit(ERR);
    }
	if ((AppState.MidWin = newwin(FULL_COORD_Y - 13, FULL_COORD_X, 3, 1)) == nullptr) {
        std::cerr << "Error! Couldn't initialise mid window!" << std::endl;
        exit(ERR);
    }

	if ((AppState.ShortcutWin = newwin(10, COLS / 2, LINES - 10, 0)) == nullptr) {
        std::cerr << "Error! Couldn't initialise shortcut window!" << std::endl;
    }

	if ((AppState.StatisticsWin = newwin(10, COLS / 2, LINES - 10, COLS / 2)) == nullptr) {
        std::cerr << "Error! Couldn't initialise statistics window!" << std::endl;
    }

    cbreak();
    noecho();
	nodelay(AppState.FullWin, true);
    curs_set(0);
}

void init() noexcept
{
    init_windows();
    init_colors();
    curs_set(0);
}

//general Layout Idea
/*
#C92020 <- Header Standard coloring(darkish red)
Timer background: #03A4BC, Timer Foreground: Black

+-----------------+------------+----------+
| (E)dit Settings | Statistics | ProgName |
+-----------------+------------+----------+
|                                         |
|    -> MODE(Pause, Long, Short, Pomo)    |
|                 TIMER                   |
|                                         |
+------------------+----------------------+
| Shortcuts:       | Short Statistics, e.g|
|  -> (B)reak      |                      |
|  -> (L)ong Break |  X Pomodoros this run|
|  -> (P)ause      |  X breaks till long  |
|  -> P(o)modoro   |  X Time estimate till|
|  -> (C)lose      |    done.             |
|                  |  X long breaks taken |
+------------------+----------------------+

*/

void winch_handle(int sig) noexcept
{
    if (sig == SIGWINCH)
		AppState._INTERRUPTED_ = true;
}

int dummy(int bla) noexcept
{
    std::cout << bla;
    return bla;
}

int main()
{
	/**
   * Here lies dragons - actually handling WINCH/Resizing without segfaults. o.o
   */

    setlocale(LC_ALL, "");
    struct sigaction sa;
    std::memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = winch_handle;
    sigaction(SIGWINCH, &sa, NULL);

    std::atomic_bool stop = false;
    std::atomic_bool globalstop = false;
    init();

    // Timer - main functionality
    PomoStatistics Stats;
    PomodoroTimer Timer { stop, Stats };
    PomodoroTimer::View AppView(Timer, Stats);

    SingleThreadLoop ThreadWrap(globalstop);

    auto interrupt_handle = [&]() {
        endwin();
        init();
        refresh();
        clear();
		getmaxyx(AppState.FullWin, FULL_COORD_Y, FULL_COORD_X);
        refresh();
		AppState._INTERRUPTED_ = false;
    };

    auto EraseStateChangeRepaint = [&]() {
		werase(AppState.MidWin);
		werase(AppState.ShortcutWin);
		werase(AppState.StatisticsWin);
		werase(AppState.TopPanel);
		AppView.Shortcut(*AppState.ShortcutWin);
		AppView.Statistcs(*AppState.StatisticsWin);
		AppView.TitleBar(*AppState.TopPanel);
    };

	auto ThreadReflectStateChanges = [&](PomodoroTimer& PomObj, std::function<void()> runFunc) {
        PomObj.Pause();
        App::delayedInsertion<std::chrono::milliseconds>(ThreadWrap, runFunc, 5);
        PomObj.Unpause();
        EraseStateChangeRepaint();
    };

    auto StateToStr = [](PomoState const& state) {
        switch (state) {
        case PomoState::LONG:
            return std::string_view(" LONG BREAK");
        case PomoState::PAUSE:
            return std::string_view(" PAUSE");
        case PomoState::POMODORO:
            return std::string_view(" POMODORO");
        case PomoState::SHORT:
            return std::string_view(" SHORT BREAK");
        case PomoState::STOP:
            return std::string_view(" STOPPED");
        default:
            return std::string_view(" none");
        }
    };

    if (!ThreadWrap.RunThread(App::HoldThread())) {
        quitter();
        ThreadWrap.Stop();
        std::cerr << "Couldn't allocate Thread!";
    }

    ThreadWrap.insert(&PomodoroTimer::RunPomo, std::ref(Timer), POMODORO_TIME);

	while (true) {
#define BIND(func, r, state) std::bind(&func, std::ref(r), state)
#define REFLECT(Timer, func, r, state) \
	ThreadReflectStateChanges(Timer, BIND(func, r, state))
		using namespace TimerApp;
        int c = wgetch(stdscr);
		AppView.Mid(*AppState.MidWin);
		AppView.Mode(*AppState.MidWin);
		AppView.Shortcut(*AppState.ShortcutWin);
		AppView.TitleBar(*AppState.TopPanel);
		AppView.Statistcs(*AppState.StatisticsWin);
#ifndef NDEBUG
		EraseSpecific(AppState.TopPanel, 0, COLS - 30);
        std::string StateLine = "Time L: " + std::to_string(Timer.getTimeLeft());
		mvwaddstr(AppState.TopPanel, 0, COLS - 40, StateLine.c_str());
        StateLine = "State: ";
        StateLine += StateToStr(Timer.getState());
        StateLine += " | Old: ";
        StateLine += StateToStr(Timer.M_oldState);
		mvwaddstr(AppState.TopPanel, 1, COLS - 40, StateLine.c_str());
		wrefresh(AppState.TopPanel);
#endif
        refresh();
        if (c == 'c') {
            Timer.Pause();
            quitter();
            if (ThreadWrap.is_running())
                ThreadWrap.Stop();
            return (EXIT_SUCCESS);
        } else if (c == 'p' && Timer.getState() != PomoState::PAUSE) {
			REFLECT(Timer, PomodoroTimer::RunPause, Timer, PAUSE_STOP_VAL);
        } else if (c == 'p' && Timer.getState() == PomoState::PAUSE) {
			ThreadReflectStateChanges(Timer, std::bind(&PomodoroTimer::RunResume, std::ref(Timer)));
		} else if (c == 'o') {
			REFLECT(Timer, PomodoroTimer::RunPomo, Timer, POMODORO_TIME);
		} else if (c == 'b') {
			REFLECT(Timer, PomodoroTimer::RunShortBreak, Timer, SHORT_BREAK_TIME);
		} else if (c == 'l') {
			REFLECT(Timer, PomodoroTimer::RunBigBreak, Timer, BIG_BREAK_TIME);
		} else if (Timer.getState() == PomoState::STOP && Timer.M_oldState != PomoState::STOP) {
			/* ^ dont stark threads over and over again. Accomplished by ^this^ check*/
			mvwaddstr(AppState.TopPanel, 0, COLS - 20, "HERE");
			wrefresh(AppState.TopPanel);
			REFLECT(Timer, PomodoroTimer::RunStop, Timer, PAUSE_STOP_VAL);
        } else if (c == KEY_RESIZE) {
            // we have to do this actually, because the signal itself isn't really portable D:
            winch_handle(SIGWINCH);
        }

		if (AppState._INTERRUPTED_) {
            interrupt_handle();
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
    return (0);
}
