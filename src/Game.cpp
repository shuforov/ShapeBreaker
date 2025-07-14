#include "../include/Game.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <math.h>
#include <memory>

Game::Game(const std::string &config) { init(config); }

void Game::init(const std::string &path) {
  // set seeds for rundomizer function
  srand(time(0));

  // Reading data in config file here
  //       using the premade PlayerConfig, EnemyConfig, BulletConfig variables
  //       to store config data

  std::ifstream fileInput(path);
  if (!fileInput.is_open()) {
    std::cerr << "Could not open config file: " << path << std::endl;
    exit(1);
  }
  std::string configName;
  int windowWidth;
  int windowHeight;
  int frameLimit;
  int screenMode;

  while (fileInput >> configName) {
    if (configName == "Window") {
      fileInput >> windowWidth >> windowHeight >> frameLimit >> screenMode;
    } else if (configName == "Player") {
      fileInput >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S >>
          m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB >>
          m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >>
          m_playerConfig.OT >> m_playerConfig.V;

    } else if (configName == "Enemy") {
      fileInput >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >>
          m_enemyConfig.SMAX >> m_enemyConfig.OR >> m_enemyConfig.OG >>
          m_enemyConfig.OB >> m_enemyConfig.OT >> m_enemyConfig.VMIN >>
          m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SI;
    } else if (configName == "Bullet") {
      fileInput >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S >>
          m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB >>
          m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB >>
          m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;
    }
  }

  // set up default window parameters
  m_window.create(sf::VideoMode(windowWidth, windowHeight), "Assignment 2");
  m_window.setFramerateLimit(frameLimit);

  spawnPlayer();
  spawnEnemy(); // spawn enemy for test TODO: remove after adding dynamic spawn
                // enemy
}

void Game::run() {
  // TODO: add pause functionality in here
  //       some systems should function while paused (rendering)
  //       some systems shouldn't (movement / input)
  while (m_running) {
    m_entities.update();

    sEnemySpawner();
    sMovement();
    sCollision();
    sUserInput();
    sRender();

    // increment the current frame
    // may need to be moved when pause implemented
    m_currentFrame++;
  }
}

void Game::setPaused(bool paused) {
  // TODO
}

// respawn the player in the middle of the screen
void Game::spawnPlayer() {
  // We create every entity by calling EntityManager.addEntity(tag)
  // This returns a std::shared_ptr<Entity>, so we use 'auto' to save typing
  auto entity = m_entities.addEntity("player");

  // Give this entity a Transform so it spawns at center of window with velocity
  // (0, 0) and angle 0
  Vec2 playerPosition;
  sf::Vector2u windowSize = m_window.getSize();
  playerPosition = Vec2(windowSize.x, windowSize.y) / 2;
  entity->cTransform =
      std::make_shared<CTransform>(playerPosition, Vec2(0.0f, 0.0f), 0.0f);

  // The entity's shape will have radius X, X sides, COLOR-X fill, and COLOR-X
  // outline of thickness X (X is number from config structure PlayerConfig)
  entity->cShape = std::make_shared<CShape>(
      m_playerConfig.SR, m_playerConfig.V,
      sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB),
      sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB),
      m_playerConfig.OT);

  // Add an input component to the player so that we can use inputs
  entity->cInput = std::make_shared<CInput>();

  // Since we want this entity to be our player, set our Game's player variable
  // to be this Entity This goes slightly against th EntityManager paradigm, but
  // we use th player so much it's worth it
  m_player = entity;
}

// spawn an enemy at a random position
void Game::spawnEnemy() {
  // Spawne enemy properly with the m_enemyConfig variables
  //       the enemy spawned completely within the bounds of the window

  auto entity = m_entities.addEntity("enemy");
  // Give this entity a Transform so it spawns at range of window with velocity
  // (0, 0) and angle 0
  Vec2 enemyPosition;
  sf::Vector2u windowSize = m_window.getSize();
  // x and y range of spawn for enemy
  sf::Vector2u xRangeWindowSpawn =
      sf::Vector2u(m_enemyConfig.SR, windowSize.x - m_enemyConfig.SR);
  sf::Vector2u yRangeWindowSpawn =
      sf::Vector2u(m_enemyConfig.SR, windowSize.y - m_enemyConfig.SR);
  int xRundNum = rundomNumber(xRangeWindowSpawn.x, xRangeWindowSpawn.y);
  int yRundNum = rundomNumber(yRangeWindowSpawn.x, yRangeWindowSpawn.y);
  int shapeVerticesRundNum =
      rundomNumber(4, 8); // rundom number of vertices for shape

  entity->cTransform = std::make_shared<CTransform>(Vec2(xRundNum, yRundNum),
                                                    Vec2(0.0f, 0.0f), 0.0f);
  entity->cShape = std::make_shared<CShape>(
      m_enemyConfig.SR, shapeVerticesRundNum, sf::Color(rundomColor()),
      sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB),
      m_enemyConfig.OT);
}

int Game::rundomNumber(int min, int max) {
  return min + (rand() % (1 + max - min));
}

sf::Color Game::rundomColor() {
  int rRundomNum = rundomNumber(0, 255);
  int gRundomNum = rundomNumber(0, 255);
  int bRundomNum = rundomNumber(0, 255);
  sf::Color result = sf::Color(rRundomNum, gRundomNum, bRundomNum);
  return result;
}

// spawns the small enemies when a big one (input entity e) explodes
void Game::spawnSmallEnemies(std::shared_ptr<Entity> e) {
  // TODO: spawn small enemies at the location of the input enemy e

  // when we create the smaller enemy, we have to read the values of the
  // original enemy
  // - spawn a number of smal enemies equal to the vertices of the original
  // enemy
  // - set each small enemy to the same color as the original, half the size
  // - small enemies are worth double points of the original enemy
}

// spawns a bullet from a given entity to a target location
void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2 &target) {
  // TODO: implement the spawning of a bullet which travels toward target
  //       - bullet speed is given as a scalar speed
  //       - you must set the velocity by using formula in notes
  auto entityBullet = m_entities.addEntity("bullet");
  Vec2 bulletPosition = entity->cTransform->pos;
  Vec2 bulletNormalize = bulletPosition.normalizeToTarget(target);
  Vec2 bulletVelocity = Vec2(bulletNormalize.x * m_bulletConfig.S,
                             bulletNormalize.y * m_bulletConfig.S);
  entityBullet->cTransform =
      std::make_shared<CTransform>(bulletPosition, bulletVelocity, 0.0f);
  entityBullet->cShape = std::make_shared<CShape>(
      m_bulletConfig.SR, m_bulletConfig.V, sf::Color::White, sf::Color::Red,
      m_bulletConfig.OT);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity) {
  // TODO: implement your own special weapon
}

void Game::sMovement() {
  // All entity movement implementation in this function

  for (auto &entity : m_entities.getEntities("bullet")) {
    entity->cTransform->pos.x += entity->cTransform->velocity.x;
    entity->cTransform->pos.y += entity->cTransform->velocity.y;
  }

  for (auto &entity : m_entities.getEntities("enemy")) {
    entity->cTransform->pos.x += entity->cTransform->velocity.x;
    entity->cTransform->pos.y += entity->cTransform->velocity.y;
  }

  if (m_player->cInput->up || m_player->cInput->down ||
      m_player->cInput->left || m_player->cInput->right) {
    if (m_player->cInput->up & !m_player->cInput->down) {
      m_player->cTransform->velocity.y = -1;
      if (m_player->cInput->right) {
        m_player->cTransform->velocity.x = 1;
      } else if (m_player->cInput->left) {
        m_player->cTransform->velocity.x = -1;
      } else {
        m_player->cTransform->velocity.x = 0;
      }
    } else if (m_player->cInput->down & !m_player->cInput->up) {
      m_player->cTransform->velocity.y = 1;
      if (m_player->cInput->right) {
        m_player->cTransform->velocity.x = 1;
      } else if (m_player->cInput->left) {
        m_player->cTransform->velocity.x = -1;
      } else {
        m_player->cTransform->velocity.x = 0;
      }
    } else {
      m_player->cTransform->velocity.y = 0;
    }
    if (m_player->cInput->left & !m_player->cInput->right) {
      m_player->cTransform->velocity.x = -1;
    } else if (m_player->cInput->right & !m_player->cInput->left) {
      m_player->cTransform->velocity.x = 1;
    } else {
      m_player->cTransform->velocity.x = 0;
    }
  } else {
    m_player->cTransform->velocity = Vec2(0, 0);
  }
  m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
  m_player->cTransform->pos.y += m_player->cTransform->velocity.y;
}

void Game::sLifespan() {
  // TODO: implement all lifespan functionality
  //
  // for all entities
  //     if entity has no lifespan component, skip it
  //     if entity has > 0 remaining lifespan, subtract 1
  //     if it has lifespan and is alive
  //         scale its alpha channel properly
  //     if it has lifespan and its time is up
  //         destroy the entity
}

void Game::sCollision() {
  // TODO: implement all proper collisions between entities
  //       be sure to use the collision radius, NOT th shape radius
}

void Game::sEnemySpawner() {
  // TODO: code which implenets enemy spawning should go here
  //
  //       (use m_currentFrame - m_lastEnemySpawnTime) to determine
  //       how long it has been since the last enemy spawned
}

void Game::sRender() {
  m_window.clear();

  // Draw ALL of the entities
  for (auto &entityNode : m_entities.getEntities()) {
    // set the poisition of the shape based on the entity's transform->pos
    entityNode->cShape->circle.setPosition(entityNode->cTransform->pos.x,
                                           entityNode->cTransform->pos.y);

    // set the rotation of the shape based on the entity's transform->angle
    entityNode->cTransform->angle += 1.0f;
    entityNode->cShape->circle.setRotation(entityNode->cTransform->angle);

    // draw the entity`s sf::CircleShape
    m_window.draw(entityNode->cShape->circle);
  }

  m_window.display();
}

void Game::sUserInput() {
  // Handle input event of player and updata player cInput component state
  // and also process mouse button input event

  sf::Event event;
  while (m_window.pollEvent(event)) {
    // this event triggers when the window is closed
    if (event.type == sf::Event::Closed) {
      m_running = false;
    }

    // this event is triggered when a key is pressed
    if (event.type == sf::Event::KeyPressed) {
      switch (event.key.code) {
      case sf::Keyboard::W:
        std::cout << "W Key Pressed\n";
        m_player->cInput->up = true;
        break;
      case sf::Keyboard::A:
        std::cout << "A Key Pressed\n";
        m_player->cInput->left = true;
        break;
      case sf::Keyboard::S:
        std::cout << "S Key Pressed\n";
        m_player->cInput->down = true;
        break;
      case sf::Keyboard::D:
        std::cout << "D Key Pressed\n";
        m_player->cInput->right = true;
        break;
      default:
        break;
      }
    }

    // this event is triggers when a key is released
    if (event.type == sf::Event::KeyReleased) {
      switch (event.key.code) {
      case sf::Keyboard::W:
        std::cout << "W Key Released" << std::endl;
        m_player->cInput->up = false;
        break;
      case sf::Keyboard::A:
        std::cout << "A Key Released\n";
        m_player->cInput->left = false;
        break;
      case sf::Keyboard::S:
        std::cout << "S Key Released\n";
        m_player->cInput->down = false;
        break;
      case sf::Keyboard::D:
        std::cout << "D Key Released\n";
        m_player->cInput->right = false;
        break;
      default:
        break;
      }
    }

    if (event.type == sf::Event::MouseButtonPressed) {
      if (event.mouseButton.button == sf::Mouse::Left) {
        std::cout << "Left Mouse Button Clicked at (" << event.mouseButton.x
                  << "," << event.mouseButton.y << ")\n";
        spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
      }

      if (event.mouseButton.button == sf::Mouse::Right) {
        std::cout << "Right Mouse Button Clicked at (" << event.mouseButton.x
                  << "," << event.mouseButton.y << ")\n";
        // call spawnSpecialWeapon here
      }
    }
  }
}
