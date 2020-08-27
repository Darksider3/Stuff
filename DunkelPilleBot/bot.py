#!/usr/bin/env python3
# bot.py
import os # for importing env vars for the bot to use
from twitchio.ext import commands
import datetime
import time
import random


NEVER_REACT_TO = ["dunkelseiter", "pilloobot"]

BEEF_STRINGS = ["!beef bastelRage2", "!beef", "!beef pillebMOPS", "!beef Clap",
        "!beef monkaS", "!beef monkaT", "!beef"]

MOPS_STRINGS = ["!mops bastelRage2", "!mops", "!mops pillebMOPS", "!mops Clap",
        "!mops monkaS", "!mops monkaT", "!mops", "!mops ROLLMOPS "]

EARLIEST_NEXT_BEEF = datetime.datetime.now() + datetime.timedelta(minutes=2)
EARLIEST_NEXT_KART = datetime.datetime.now() + datetime.timedelta(minutes=3)
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


def sleeper(WantRandom=False, lower=2, upper=5):
    time_to_wait = 2
    if WantRandom:
        time_to_wait = random.randint(lower, upper)

    time.sleep(time_to_wait)


@bot.event
async def event_message(ctx):
    global IN_BEEF
    global EARLIEST_NEXT_BEEF
    global EARLIEST_NEXT_KARAOKE
    global EARLIEST_NEXT_MOPS
    global EARLIEST_NEXT_KART
    global BEEF_STRINGS
    'Runs every time a message is sent in chat.'
    # make sure the bot ignores itself and the streamer
    # if ctx.author.name.lower() == os.environ['BOT_NICK'].lower():
    #    return
    if ctx.author.name in NEVER_REACT_TO:
        return

    if "!beef" in ctx.content: 
        if EARLIEST_NEXT_BEEF < datetime.datetime.now():
            EARLIEST_NEXT_BEEF = datetime.datetime.now() +\
                datetime.timedelta(minutes=6)
            sleeper(True)
            await ctx.channel.send(random.choice(BEEF_STRINGS))
            print(f"{datetime.datetime.now()} - DEBUG: BEEF TRIGGERED")
    if "!kart" in ctx.content: 
        if EARLIEST_NEXT_KART < datetime.datetime.now():
            EARLIEST_NEXT_KART = datetime.datetime.now() +\
                datetime.timedelta(minutes=10)
            sleeper(True)
            await ctx.channel.send("!kart")
            print(f"{datetime.datetime.now()} - DEBUG: BEEF TRIGGERED")
    if "!karaoke" in ctx.content:
        if EARLIEST_NEXT_KARAOKE < datetime.datetime.now():
            EARLIEST_NEXT_KARAOKE = datetime.datetime.now() + \
                datetime.timedelta(minutes=10)
            sleeper(True)
            await ctx.channel.send("!karaoke 666")
            print(f"{datetime.datetime.now()} - DEBUG: KARAOKE TRIGGERED")

    if EARLIEST_NEXT_MOPS < datetime.datetime.now():
        sleeper(True)
        await ctx.channel.send(random.choice(MOPS_STRINGS))
        EARLIEST_NEXT_MOPS = datetime.datetime.now() + datetime.timedelta(minutes=3)
        print(f"{datetime.datetime.now()} - DEBUG: MOPS TRIGGERED")


# bot.py
if __name__ == "__main__":
    bot.run()

