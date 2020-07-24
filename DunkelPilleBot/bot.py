# bot.py
import os # for importing env vars for the bot to use
from twitchio.ext import commands
import datetime
import time
import random

BEEF_STRINGS = ["!beef bastelRage2", "!beef", "!beef pillebMOPS", "!beef Clap",
                "!beef monkaS", "!beef monkaT", "!beef"]

MOPS_STRINGS = ["!mops bastelRage2", "!mops", "!mops pillebMOPS", "!mops Clap",
                "!mops monkaS", "!mops monkaT", "!mops", "!mops ROLLMOPS "]

EARLIEST_NEXT_BEEF = datetime.datetime.now() + datetime.timedelta(minutes=2)
EARLIEST_NEXT_KARAOKE = datetime.datetime.now() + datetime.timedelta(minutes=3)
EARLIEST_NEXT_MOPS = datetime.datetime.now() + datetime.timedelta(minutes=1)

assert(EARLIEST_NEXT_BEEF > datetime.datetime.now())

bot = commands.Bot(
    # set up the bot
    irc_token=os.environ['TMI_TOKEN'],
    client_id=os.environ['CLIENT_ID'],
    nick=os.environ['BOT_NICK'],
    prefix=os.environ['BOT_PREFIX'],
    initial_channels=[os.environ['CHANNEL']]
)

@bot.event
async def event_ready():
    'Called once when the bot goes online.'
    print(f"{os.environ['BOT_NICK']} is online!")
    ws = bot._ws  # this is only needed to send messages within event_ready
    # await ws.send_privmsg(os.environ['CHANNEL'], f"/me has landed!")

@bot.event
async def event_message(ctx):
    global IN_BEEF
    global EARLIEST_NEXT_BEEF
    global EARLIEST_NEXT_KARAOKE
    global EARLIEST_NEXT_MOPS
    global BEEF_STRINGS
    'Runs every time a message is sent in chat.'
    # make sure the bot ignores itself and the streamer
    #if ctx.author.name.lower() == os.environ['BOT_NICK'].lower():
    #    return
    if "!beef" in ctx.content: 
        if EARLIEST_NEXT_BEEF < datetime.datetime.now():
            if "pilloobot" not in ctx.author.name and "Dunkelseiter" not in ctx.author.name:
                time.sleep(2)
                await ctx.channel.send(random.choice(BEEF_STRINGS))
                EARLIEST_NEXT_BEEF=datetime.datetime.now() + datetime.timedelta(minutes=6)
                print(f"{datetime.datetime.now()} - DEBUG: BEEF TRIGGERED")
                
    if "!karaoke" in ctx.content: 
        if EARLIEST_NEXT_KARAOKE < datetime.datetime.now():
            if "pilloobot" not in ctx.author.name and "Dunkelseiter" not in ctx.author.name:
                time.sleep(2)
                await ctx.channel.send("!karaoke 666")
                EARLIEST_NEXT_KARAOKE=datetime.datetime.now() + datetime.timedelta(minutes=10)
                print(f"{datetime.datetime.now()} - DEBUG: KARAOKE TRIGGERED")
                
    if EARLIEST_NEXT_MOPS < datetime.datetime.now():
        if "pilloobot" not in ctx.author.name and "Dunkelseiter" not in ctx.author.name:
            time.sleep(2)
            await ctx.channel.send(random.choice(MOPS_STRINGS))
            EARLIEST_NEXT_MOPS=datetime.datetime.now() + datetime.timedelta(minutes=3)
            print(f"{datetime.datetime.now()} - DEBUG: MOPS TRIGGERED")


# bot.py
if __name__ == "__main__":
    bot.run()

