#!/usr/bin/env python3
# bot.py
import os # for importing env vars for the bot to use
from twitchio.ext import commands
import datetime
import time
import random


class EmojiSelector:
  Emojis = list()
  
  def __init__(self, EmojisList: list):
    self.Emojis = EmojisList;
  
  def PickRandom(self):
    return random.choice(self.Emojis)


class RunCommand:
  runnable: bool = True
  cmd: str = "!karma"
  cmd_offset_time: int = 0
  last_ran = 0
  
  random_addition = EmojiSelector
  
  def __init__(self):
    self.last_ran = datetime.datetime.now()
  
  def Command(self, ctx):
    if self.can_run():
      CmdStr = self.cmd + " " + self.random_addition.PickRandom()
      print("could run!")
      print("CMD: " + CmdStr)
      self.last_ran = datetime.datetime.now()
    else:
      print("couldnt run, time not yet: "+ self.cmd)
      
  def can_run(self):
    t_check = self.last_ran + datetime.timedelta(minutes=self.cmd_offset_time)
    self.runnable = datetime.datetime.now() > t_check 
    return self.runnable


class MopsCommand(RunCommand):
  def __init__(self, emojis):
    super().__init__()
    self.cmd = "!mops"
    self.random_addition = emojis
    self.cmd_offset_time = random.randint(2, 4)
    self.last_ran = datetime.datetime.now() - datetime.timedelta(minutes=2)
    self.successful = False # @TODO: Read from bots response wether it worked or not... and delay further
    
    
  def Mops(self, ctx):
    self.Command(ctx);
    

Emojis: list = ["hi", "world", "123"]
Emotes = EmojiSelector(
      ["bastelRage2", "bastelRage",
       "draconRage", "MOPS", "ROLLMOPS"])

Mops = MopsCommand(Emotes);


bot = commands.Bot(
        # set up the bot
        irc_token=os.environ['TMI_TOKEN'],
        client_id=os.environ['CLIENT_ID'],
        nick=os.environ['BOT_NICK'],
        prefix=os.environ['BOT_PREFIX'],
        initial_channels=[os.environ['CHANNEL']]
        )
        
@bot.event
async def event_message(ctx):
  global Emojis
  global Emotes
  global Mops
  if Mops.can_run():
    #Mops.Mops(ctx)
    print("YES")

@bot.event
async def event_ready():
    'Called once when the bot goes online.'
    print(f"{os.environ['BOT_NICK']} is online!")
    ws = bot._ws  # this is only needed to send messages within event_ready
    # await ws.send_privmsg(os.environ['CHANNEL'], f"/me has landed!")

# bot.py
if __name__ == "__main__":
    bot.run()
