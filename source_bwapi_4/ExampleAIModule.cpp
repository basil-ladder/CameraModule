#include "ExampleAIModule.h"
#include <iostream>

using namespace BWAPI;
using namespace Filter;

extern "C" void gameInit(BWAPI::Game *game) { BWAPI::BroodwarPtr = game; }
extern "C" BWAPI::AIModule *newAIModule()
{
  return new ExampleAIModule();
}

std::string env(std::string name, std::string def)
{
  const char *s = std::getenv(name.c_str());
  if (!s)
    return def;
  return s;
}


void ExampleAIModule::onStart()
{
  auto startPosition = Position(0, 0);
  auto players = Broodwar->getPlayers();
  for (auto iter = players.cbegin(); iter != players.end(); ++iter)
  {
    if (!(*iter)->isObserver() && !(*iter)->isNeutral())
    {
      startPosition = Position((*iter)->getStartLocation());
    }
  }

  cameraModule.onStart(startPosition, std::stoi(env("OPENBW_SCREEN_WIDTH", "1280")), std::stoi(env("OPENBW_SCREEN_HEIGHT", "720")));
}

void ExampleAIModule::onEnd(bool isWinner)
{
}

void ExampleAIModule::onFrame()
{
  cameraModule.onFrame();
  Broodwar->setTextSize(Text::Size::Large);
  Broodwar->drawTextScreen(Positions::Origin, "%c%c%s", Text::Align_Center, Text::Green, "GOSU!");
  Broodwar->setTextSize();
}

void ExampleAIModule::onSendText(std::string text)
{

  // Send the text to the game if it is not being processed.
  Broodwar->sendText("%s", text.c_str());

  // Make sure to use %s and pass the text as a parameter,
  // otherwise you may run into problems when you use the %(percent) character!
}

void ExampleAIModule::onReceiveText(BWAPI::Player player, std::string text)
{
  // Parse the received text
  Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void ExampleAIModule::onPlayerLeft(BWAPI::Player player)
{
}

void ExampleAIModule::onNukeDetect(BWAPI::Position target)
{

  // Check if the target is a valid position
  if (target)
  {
    // if so, print the location of the nuclear strike target
    Broodwar << "Nuclear Launch Detected at " << target << std::endl;
  }
  else
  {
    // Otherwise, ask other players where the nuke is!
    Broodwar->sendText("Where's the nuke?");
  }

  // You can also retrieve all the nuclear missile targets using Broodwar->getNukeDots()!
}

void ExampleAIModule::onUnitDiscover(BWAPI::Unit unit)
{
}

void ExampleAIModule::onUnitEvade(BWAPI::Unit unit)
{
}

void ExampleAIModule::onUnitShow(BWAPI::Unit unit)
{
}

void ExampleAIModule::onUnitHide(BWAPI::Unit unit)
{
}

void ExampleAIModule::onUnitCreate(BWAPI::Unit unit)
{
  if (Broodwar->isReplay())
  {
    // if we are in a replay, then we will print out the build order of the structures
    if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral())
    {
      int seconds = Broodwar->getFrameCount() / 24;
      int minutes = seconds / 60;
      seconds %= 60;
      Broodwar->sendText("%.2d:%.2d: %s creates a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
    }
  }
}

void ExampleAIModule::onUnitDestroy(BWAPI::Unit unit)
{
}

void ExampleAIModule::onUnitMorph(BWAPI::Unit unit)
{
  if (Broodwar->isReplay())
  {
    // if we are in a replay, then we will print out the build order of the structures
    if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral())
    {
      int seconds = Broodwar->getFrameCount() / 24;
      int minutes = seconds / 60;
      seconds %= 60;
      Broodwar->sendText("%.2d:%.2d: %s morphs a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
    }
  }
}

void ExampleAIModule::onUnitRenegade(BWAPI::Unit unit)
{
}

void ExampleAIModule::onSaveGame(std::string gameName)
{
  Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void ExampleAIModule::onUnitComplete(BWAPI::Unit unit)
{
  cameraModule.moveCameraUnitCreated(unit);
}
