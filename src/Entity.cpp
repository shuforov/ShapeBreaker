#include <string>

#include "../include/Entity.h"

Entity::Entity(const size_t id, const std::string &tag)
    : m_id(id), m_tag(tag) {}

bool Entity::isActive() const { return m_active; }

const std::string &Entity::tag() const { return m_tag; }

const size_t Entity::id() const { return m_id; }

void Entity::destroy() { m_active = false; }

void Entity::updateLifeSpanAndColor() {
  int lifeSpanValue = cLifespan->remaining;
  int lifeSpanTotal = cLifespan->total;
  if (lifeSpanValue > 0) {
    float progress = static_cast<float>(lifeSpanValue) / lifeSpanTotal;
    sf::Uint8 alpha = static_cast<sf::Uint8>(255 * (progress));
    sf::Color entityColor = cShape->circle.getFillColor();
    sf::Color entityOutLineColor = cShape->circle.getOutlineColor();
    entityColor.a = alpha;
    entityOutLineColor.a = alpha;
    cShape->circle.setFillColor(entityColor);
    cShape->circle.setOutlineColor(entityOutLineColor);
    cLifespan->remaining--;
  } else if (lifeSpanValue <= 0) {
    destroy();
  }
}
