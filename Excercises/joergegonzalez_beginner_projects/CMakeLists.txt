cmake_minimum_required(VERSION 3.14)
project(joergegonzalez_beginner_projects VERSION 0.01
        DESCRIPTION "Beginner Projects"
        LANGUAGES   CXX)


set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
add_compile_options(-lbsd)
add_link_options(-lbsd)
add_executable(base_jumper base_jumper.cpp)
add_executable(BaseDeriveDynamic BaseDeriveDynamic.cpp)
add_executable(change_calculator change_calculator.cpp)
add_executable(coin_estimator_by_weight coin_estimator_by_weight.cpp)
add_executable(dice_roll_sim dice_rolling_sim.cpp)
add_executable(fibonacci fibonacci.cpp)
add_executable(hangman hangman_game.cpp)
add_executable(higher_lower higher_lower_guessing_game.cpp)
add_executable(magic_8_ball magic_8_ball.cpp)
add_executable(mean_median_mode mean_median_mode.cpp)
add_executable(menu_calc menu_calc.cpp)
add_executable(palindrome palindrome.cpp)
add_executable(progressbar progressbar.cpp)
add_executable(pythagorean_triples_checker pythagorean_triples_checker.cpp)
add_executable(rock_paper_scissors rock_paper_scissors.cpp)
add_executable(factors_of_my_number factors_of_my_number.cpp)
