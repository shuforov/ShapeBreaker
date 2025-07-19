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
#include <string>

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
    } else if (configName == "Font") {
      std::string fontPath;
      int fontSize;
      sf::Color fontColor;
      fileInput >> fontPath >> fontSize >> fontColor.r >> fontColor.g >>
          fontColor.b;
      if (!m_font.loadFromFile(fontPath)) {
        throw std::invalid_argument("Font not loaded");
      }
      m_text.setFont(m_font);
      m_text.setCharacterSize(fontSize);
      m_text.setFillColor(fontColor);
    }
  }

  // set up default window parameters
  m_window.create(sf::VideoMode(windowWidth, windowHeight), "Assignment 2");
  m_window.setFramerateLimit(frameLimit);

  spawnPlayer();
}

void Game::run() {
  while (m_running) {
    m_entities.update();

    sUserInput();
    sRender();
    if (!m_paused) {
      sMovement();
      sLifespan();
      sEnemySpawner();
      sCollision();
    }
    // increment the current frame
    // may need to be moved when pause implemented
    m_currentFrame++;
  }
}

void Game::setPaused(bool paused) { m_paused = paused; }

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
  entity->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);
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
      rundomNumber(3, 8); // rundom number of vertices for shape

  entity->cTransform = std::make_shared<CTransform>(Vec2(xRundNum, yRundNum),
                                                    rundomVelocity(), 1.0f);
  entity->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);
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

Vec2 Game::rundomVelocity() {
  int xValue = rand() % 2 == 0 ? -1 : 1;
  int yValue = rand() % 2 == 0 ? -1 : 1;
  return Vec2(xValue, yValue);
}

// spawns the small enemies when a big one (input entity e) explodes
void Game::spawnSmallEnemies(std::shared_ptr<Entity> e) {
  int movementSpeed = 5; // movement speed of spawned small enemy
  int shapeVertices = e->cShape->circle.getPointCount();
  int shapeSize = e->cShape->circle.getRadius() / 2;
  Vec2 positionEnemey = e->cTransform->pos;
  int angleSide = 360 / shapeVertices;
  // creating small enemies process loop
  for (int i = 1; i <= shapeVertices; ++i) {
    int angleStepRadius = angleSide * i;
    float angleRdaius = angleStepRadius * (M_PI / 180);
    Vec2 targetPoint = Vec2(cos(angleRdaius), sin(angleRdaius));
    Vec2 velocityValue =
        Vec2(targetPoint.x * movementSpeed, targetPoint.y * movementSpeed);
    // creating small enemy
    auto entity = m_entities.addEntity("smallEnemy");
    entity->cTransform =
        std::make_shared<CTransform>(positionEnemey, velocityValue, 0.0f);
    entity->cShape = std::make_shared<CShape>(
        m_enemyConfig.SR / 2.0f, shapeVertices,
        e->cShape->circle.getFillColor(), e->cShape->circle.getOutlineColor(),
        e->cShape->circle.getOutlineThickness());
    entity->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);
    entity->cCollision = std::make_shared<CCollision>(e->cCollision->radius);
  }
}

// spawns a bullet from a given entity to a target location
void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2 &target) {
  auto entityBullet = m_entities.addEntity("bullet");
  Vec2 bulletPosition = entity->cTransform->pos;
  Vec2 bulletNormalize = bulletPosition.normalizeToTarget(target);
  Vec2 bulletVelocity = Vec2(bulletNormalize.x * m_bulletConfig.S,
                             bulletNormalize.y * m_bulletConfig.S);
  entityBullet->cTransform =
      std::make_shared<CTransform>(bulletPosition, bulletVelocity, 0.0f);
  entityBullet->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
  entityBullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
  entityBullet->cShape = std::make_shared<CShape>(
      m_bulletConfig.SR, m_bulletConfig.V, sf::Color::White, sf::Color::Red,
      m_bulletConfig.OT);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> e) {
  //  processing of special weapon
  int movementSpeed = m_bulletConfig.S; // movement speed of bullet
  int shapeVertices = e->cShape->circle.getPointCount();
  int shapeSize = e->cShape->circle.getRadius() / 2;
  Vec2 positionPlayer = e->cTransform->pos;
  int angleSide = 360 / shapeVertices;
  // creating bullets process loop
  for (int i = 1; i <= shapeVertices; ++i) {
    int angleStepRadius = angleSide * i;
    float angleRdaius = angleStepRadius * (M_PI / 180);
    Vec2 targetPoint = Vec2(cos(angleRdaius), sin(angleRdaius));
    Vec2 velocityValue =
        Vec2(targetPoint.x * movementSpeed, targetPoint.y * movementSpeed);
    // creating bullet
    auto entity = m_entities.addEntity("bullet");
    entity->cTransform =
        std::make_shared<CTransform>(positionPlayer, velocityValue, 0.0f);
    entity->cShape = std::make_shared<CShape>(
        m_enemyConfig.SR / 2.0f, shapeVertices, sf::Color::White,
        e->cShape->circle.getOutlineColor(),
        e->cShape->circle.getOutlineThickness());
    entity->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);
    entity->cCollision = std::make_shared<CCollision>(e->cCollision->radius);
  }
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

  for (auto &entity : m_entities.getEntities("smallEnemy")) {
    entity->cTransform->pos.x += entity->cTransform->velocity.x;
    entity->cTransform->pos.y += entity->cTransform->velocity.y;
  }

  sPlayerInputStateProcess();
}

void Game::sPlayerInputStateProcess() {
  // Process player inputs state and change velocity based on input state.
  if (m_paused) {
    return;
  }

  sf::Vector2u xRangeWindowSpawn =
      sf::Vector2u(m_playerConfig.SR, m_window.getSize().x - m_playerConfig.SR);
  sf::Vector2u yRangeWindowSpawn =
      sf::Vector2u(m_playerConfig.SR, m_window.getSize().y - m_playerConfig.SR);

  Vec2 topLeftLimit = Vec2(m_playerConfig.SR, m_playerConfig.SR);
  Vec2 bottomRightLimit = Vec2(m_window.getSize().x - m_playerConfig.SR,
                               m_window.getSize().y - m_playerConfig.SR);

  Vec2 currentPosition = m_player->cTransform->pos;

  if (m_player->cInput->up || m_player->cInput->down ||
      m_player->cInput->left || m_player->cInput->right) {
    if (m_player->cInput->up & !m_player->cInput->down) {
      if (currentPosition.y < m_playerConfig.SR) {
        // it's collide with up edge it can't move up
        m_player->cInput->up = false;
      } else {
        m_player->cTransform->velocity.y = -1;
      }
      if (m_player->cInput->right) {
        if (currentPosition.x > m_window.getSize().x - m_playerConfig.SR) {
          // it's collide with right edge it can't move right
          m_player->cInput->right = false;
        } else {
          m_player->cTransform->velocity.x = 1;
        }
      } else if (m_player->cInput->left) {
        if (currentPosition.x < m_playerConfig.SR) {
          // it's collide with left edge it can't move left
          m_player->cInput->left = false;
        } else {
          m_player->cTransform->velocity.x = -1;
        }
      } else {
        m_player->cTransform->velocity.x = 0;
      }
    } else if (m_player->cInput->down & !m_player->cInput->up) {
      if (currentPosition.y > m_window.getSize().y - m_playerConfig.SR) {
        // it's collide with bottom edge it can't move bottom
        m_player->cInput->down = false;
      } else {
        m_player->cTransform->velocity.y = 1;
      }
      if (m_player->cInput->right) {
        if (currentPosition.x > m_window.getSize().x - m_playerConfig.SR) {
          // it's collide with right edge it can't move right
          m_player->cInput->right = false;
        } else {
          m_player->cTransform->velocity.x = 1;
        }
      } else if (m_player->cInput->left) {
        if (currentPosition.x < m_playerConfig.SR) {
          // it's collide with left edge it can't move left
          m_player->cInput->left = false;
        } else {
          m_player->cTransform->velocity.x = -1;
        }
      } else {
        m_player->cTransform->velocity.x = 0;
      }
    } else {
      m_player->cTransform->velocity.y = 0;
    }
    if (m_player->cInput->left & !m_player->cInput->right) {
      if (currentPosition.x < m_playerConfig.SR) {
        // it's collide with left edge it can't move left
        m_player->cInput->left = false;
      } else {
        m_player->cTransform->velocity.x = -1;
      }
    } else if (m_player->cInput->right & !m_player->cInput->left) {
      if (currentPosition.x > m_window.getSize().x - m_playerConfig.SR) {
        // it's collide with right edge it can't move right
        m_player->cInput->right = false;
      } else {
        m_player->cTransform->velocity.x = 1;
      }
    } else {
      m_player->cTransform->velocity.x = 0;
    }
  } else {
    m_player->cTransform->velocity = Vec2(0, 0);
  }
  m_player->cTransform->pos.x +=
      m_player->cTransform->velocity.x * m_playerConfig.S;
  m_player->cTransform->pos.y +=
      m_player->cTransform->velocity.y * m_playerConfig.S;
}

void Game::sLifespan() {
  // update lifetime counter for bullets
  for (auto &entity : m_entities.getEntities("bullet")) {
    entity->updateLifeSpanAndColor();
  }
  // update lifetime counter for small enemy
  for (auto &entity : m_entities.getEntities("smallEnemy")) {
    entity->updateLifeSpanAndColor();
  }
}

void Game::sCollision() {
  // Implementation of all proper collisions between entities

  for (auto entityBullet : m_entities.getEntities("bullet")) {
    for (auto entityEnemy : m_entities.getEntities("enemy")) {
      float distBE = entityBullet->cTransform->pos.dist(
          entityEnemy->cTransform
              ->pos); // get distance between Bullet and Enemy nodes.
      float summRadius =
          entityBullet->cCollision->radius +
          entityEnemy->cCollision
              ->radius; // get summ of radius of Bullet and Enemy.
      if (distBE < summRadius) {
        spawnSmallEnemies(entityEnemy);
        entityEnemy->destroy();
        entityBullet->destroy();
        m_score += enemyScorePoints;
      }
    }

    for (auto entitySmallEnemy : m_entities.getEntities("smallEnemy")) {
      float distBSE = entityBullet->cTransform->pos.dist(
          entitySmallEnemy->cTransform
              ->pos); // get distance between Bullet and Small Enemy nodes.
      float summRadius =
          entityBullet->cCollision->radius +
          entitySmallEnemy->cCollision
              ->radius; // get summ of radius of Bullet and Small Enemy.
      if (distBSE < summRadius) {
        entitySmallEnemy->destroy();
        entityBullet->destroy();
        m_score += smallEnemyScorePoints;
      }
    }
  }

  sf::Vector2u xRangeWindowSpawn =
      sf::Vector2u(m_enemyConfig.SR, m_window.getSize().x - m_enemyConfig.SR);
  sf::Vector2u yRangeWindowSpawn =
      sf::Vector2u(m_enemyConfig.SR, m_window.getSize().y - m_enemyConfig.SR);

  Vec2 topLeftLimit = Vec2(m_enemyConfig.SR, m_enemyConfig.SR);
  Vec2 bottomRightLimit = Vec2(m_window.getSize().x - m_enemyConfig.SR,
                               m_window.getSize().y - m_enemyConfig.SR);

  // check if enemy object incide of window range if not then it should bounce
  // from edge of the window
  for (auto entityEnemy : m_entities.getEntities("enemy")) {
    if (!(entityEnemy->cTransform->pos.x >= topLeftLimit.x &&
          entityEnemy->cTransform->pos.x <= bottomRightLimit.x) ||
        !(entityEnemy->cTransform->pos.y >= topLeftLimit.y &&
          entityEnemy->cTransform->pos.y <= bottomRightLimit.y)) {
      // check with which edge colliding enemy
      Vec2 currentPosition = entityEnemy->cTransform->pos;
      if (currentPosition.y > m_window.getSize().y - m_enemyConfig.SR &&
          currentPosition.x < m_window.getSize().x - m_enemyConfig.SR) {
        // it's collide with bottom edge
        entityEnemy->cTransform->velocity.y = -1;
      } else if (currentPosition.y < m_enemyConfig.SR &&
                 currentPosition.x < m_window.getSize().x - m_enemyConfig.SR) {
        // it's collide with up edge
        entityEnemy->cTransform->velocity.y = 1;
      } else if (currentPosition.y < m_window.getSize().y - m_enemyConfig.SR &&
                 currentPosition.x < m_enemyConfig.SR) {
        // it's collide with left edge
        Vec2 velocityValue = entityEnemy->cTransform->velocity;
        entityEnemy->cTransform->velocity.x = 1;

      } else if (currentPosition.y < m_window.getSize().y - m_enemyConfig.SR &&
                 currentPosition.x > m_window.getSize().x - m_enemyConfig.SR) {
        // it's collide with right edge
        entityEnemy->cTransform->velocity.x = -1;
      }
    }
    // Check if enemy colliding with player and if it's collide player should
    // respawn at center of screen
    float distPE = m_player->cTransform->pos.dist(
        entityEnemy->cTransform
            ->pos); // get distance between Player and Enemy nodes.
    float summRadius = m_player->cCollision->radius +
                       entityEnemy->cCollision
                           ->radius; // get summ of radius of Player and Enemy.
    if (distPE < summRadius) {
      entityEnemy->destroy();
      m_player->destroy();
      spawnPlayer();
    }
  }
  // check if small enemy collide with player
  for (auto entitySmallEnemy : m_entities.getEntities("smallEnemy")) {
    float distPSE = m_player->cTransform->pos.dist(
        entitySmallEnemy->cTransform
            ->pos); // get distance between player and small enemy nodes
    float summRadius = m_player->cCollision->radius +
                       entitySmallEnemy->cCollision
                           ->radius; // get summ of radius of player and enemy
    if (distPSE < summRadius) {
      entitySmallEnemy->destroy();
      m_player->destroy();
      spawnPlayer();
    }
  }
}

void Game::sEnemySpawner() {
  // logic of spawning enemy every m_enemyConfig.SI time
  if (m_lastEnemySpawnTime < m_enemyConfig.SI) {
    m_lastEnemySpawnTime++;
  } else if (m_lastEnemySpawnTime == m_enemyConfig.SI) {
    spawnEnemy();
    m_lastEnemySpawnTime = 0;
  }
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
  // draw text score
  m_text.setString("Score points: " + std::to_string(m_score));
  m_window.draw(m_text);

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
        m_player->cInput->up = true;
        break;
      case sf::Keyboard::A:
        m_player->cInput->left = true;
        break;
      case sf::Keyboard::S:
        m_player->cInput->down = true;
        break;
      case sf::Keyboard::D:
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
        m_player->cInput->up = false;
        break;
      case sf::Keyboard::A:
        m_player->cInput->left = false;
        break;
      case sf::Keyboard::S:
        m_player->cInput->down = false;
        break;
      case sf::Keyboard::D:
        m_player->cInput->right = false;
        break;
      default:
        break;
      }
    }

    if (event.type == sf::Event::MouseButtonPressed) {
      if (event.mouseButton.button == sf::Mouse::Left) {
        if (m_paused) {
          return;
        }
        spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
      }

      if (event.mouseButton.button == sf::Mouse::Right) {
        if (m_paused) {
          return;
        }
        spawnSpecialWeapon(m_player);
      }
    }

    if (event.type == sf::Event::KeyPressed) {
      switch (event.key.code) {
      case sf::Keyboard::P:
        setPaused(!m_paused);
        break;
      default:
        break;
      }
    }
  }
}
