#include "ExampleAIModule.h"
#include <iostream>
#include <iomanip>
#include <numeric>

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
  screen_width = 0;
}

void ExampleAIModule::onEnd(bool isWinner)
{
}

inline int getCharWidth(uint8_t c, uint8_t bSize)
{
  if (c == '\t')
    return 12 * bSize;
  if (c < ' ')
    return 0;
  if (c == ' ')
    return 3 * bSize;
  return 6 * bSize;
}

inline int getTextWidth(const char *pszStr, uint8_t bSize)
{
  // Retrieve size
  int size = 0;
  for (; *pszStr; ++pszStr)
    size += getCharWidth(*pszStr, bSize);

  return size;
}

void ExampleAIModule::onFrame()
{
  if (!screen_width)
  {
#ifdef OPENBW
    screen_width = Broodwar->getScreenSize().x;
    screen_height = Broodwar->getScreenSize().y;
#else
    screen_width = std::stoi(env("SCREEN_WIDTH", "1280"));
    screen_height = std::stoi(env("SCREEN_HEIGHT", "720"));
#endif
    if (screen_width)
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
      cameraModule.onStart(startPosition, screen_width, screen_height);
    }
    else
    {
      return;
    }
  }

  auto players = Broodwar->getPlayers();
  std::vector<std::string> lines;
  auto maxLength = 0;
  for (auto iter = players.cbegin(); iter != players.end(); ++iter)
  {
    auto player = *iter;
    if (!player->isObserver() && !player->isNeutral())
    {
      std::stringstream line;
      line << std::setfill('\x80') << Text::Align_Right;
      line << player->getTextColor();
      line << player->getName();
      if (cameraModule.hasVision(player))
        line << "*";
      else
        line << "\x80";
      line << std::left;

      auto playerUnits = player->getUnits();
      auto workers = std::count_if(playerUnits.cbegin(), playerUnits.cend(), [](BWAPI::Unit unit) { return unit->isCompleted() && unit->getType().isWorker(); });
      auto armySupply = std::accumulate(playerUnits.cbegin(), playerUnits.cend(), 0, [this](int sum, BWAPI::Unit unit) {
                          return sum + (unit->isCompleted() && cameraModule.isArmyUnit(unit) ? unit->getType().supplyRequired() : 0);
                        }) /
                        2.0;

      line << "   m " << std::setw(4) << player->minerals();
      line << "   g " << std::setw(4) << player->gas();
      line << "   s " << std::setw(3) << (player->supplyUsed() + 1) / 2 << "/" << std::setw(3) << player->supplyTotal() / 2;
      line << "   workers " << std::setw(3) << workers;
      line << "   army " << std::setw(3) << armySupply;
      auto linestr = line.str();
      auto strlen = getTextWidth(linestr.c_str(), Text::Size::Large);
      maxLength = maxLength > strlen ? maxLength : strlen;
      lines.push_back(linestr);
    }
  }
  Broodwar->drawBoxScreen(screen_width - maxLength - 16, 24, screen_width, 24 + 36, Colors::Black, true);
  Broodwar->setTextSize(Text::Size::Large);

  for (size_t y = 0; y < lines.size(); y++)
  {
    Broodwar->drawTextScreen(Position(0, y * 16 + 26), lines[y].c_str());
  }

  //Broodwar->drawTextScreen(Position(0, 3), vsInfo.str().c_str());
  Broodwar->setTextSize();

  cameraModule.onFrame();
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
