#include "../include/EntityManager.h"
#include <iostream>
#include <memory>

EntityManager::EntityManager() {}

void EntityManager::update() {
  // Adding entities from m_entitiesToAdd the proper location(s)
  //       - adding them to the vector of all entities
  //       - adding them to the vector inside the map, with the tag as a key
  for (auto &entityVector : m_entitiesToAdd) {
    m_entities.push_back(entityVector);
    m_entityMap[entityVector->m_tag].push_back(entityVector);
  }
  m_entitiesToAdd.clear();

  // remove dead entities from the vector of all entities
  removeDeadEntities(m_entities);

  // remove dead entities from each vector in the entity map
  // C++17 way of iterating through [key,value] pairs in a map
  for (auto &[tag, entityVec] : m_entityMap) {
    removeDeadEntities(entityVec);
  }
}

void EntityManager::removeDeadEntities(EntityVec &vec) {
  // Remove all dead entities from the input vector
  //       this is called by the update() function
  std::vector<size_t> entityIds;
  for (auto &entity : vec) {
    if (!entity->isActive()) {
      entityIds.push_back(entity->id());
    }
  }
  auto filteredList = std::remove_if(vec.begin(), vec.end(),
                                     [](const std::shared_ptr<Entity> &entity) {
                                       return !entity->isActive();
                                     });
  vec.erase(filteredList, vec.end());
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string &tag) {
  auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));

  m_entitiesToAdd.push_back(entity);

  return entity;
}

const EntityVec &EntityManager::getEntities() { return m_entities; }

const EntityVec &EntityManager::getEntities(const std::string &tag) {
  return m_entityMap[tag];
}
