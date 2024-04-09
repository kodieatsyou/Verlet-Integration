#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>

struct VerletObject
{
	sf::Vector2f position;
	sf::Vector2f position_last;
	sf::Vector2f acceleration;
	float objectRadius = 10.0f;
	sf::Color objectColor = sf::Color::White;

	VerletObject() = default;
	VerletObject(sf::Vector2f position_, float radius_)
		: position{ position_ }
		, position_last{ position_ }
		, acceleration{ 0.0f, 0.0f }
		, objectRadius{ radius_ }
	{}

	void update(float dt) {
		const sf::Vector2f displacement = position - position_last;
		position_last = position;
		position = position + displacement + acceleration * (dt * dt);
		acceleration = {};
	}

	void accelerate(sf::Vector2f acceleration_) {
		acceleration += acceleration_;
	}

	void setVelocity(sf::Vector2f velocity_, float dt) {
		position_last = position - (velocity_ * dt);
	}

	void addVelocity(sf::Vector2f velocity_, float dt) {
		position_last -= velocity_ * dt;
	}

	[[nodiscard]]
	sf::Vector2f getVelocity(float dt) const {
		return (position - position_last) / dt;
	}
};

class VerletSolver
{
public:
	VerletSolver() = default;

	VerletObject& addObject(sf::Vector2f position, float radius) {
		objects.emplace_back(position, radius);
		return objects.back();
	}

	uint32_t getObjectCount() {
		return objects.size();
	}

	const std::vector<VerletObject>& getObjects() const {
		return objects;
	}

	void update() {
		time += frameDt;
		const float step_dt = getStepDt();
		for (uint32_t i{ subSteps }; i--;) {
			applyGravity();
			solveCollisions();
			applyConstraint();
			updateObjects(step_dt);
		}
	}

	void setSubStepCount(uint32_t subSteps_) {
		subSteps = subSteps_;
	}

	void setSimulationUpdateRate(uint32_t rate) {
		frameDt = 1.0f / static_cast<float>(rate);
	}

	void setConstraint(sf::Vector2f center, float radius) {
		constraintCenter = center;
		constraintRadius = radius;
	}

	void setObjectVelocity(VerletObject& obj, sf::Vector2f velocity) {
		obj.setVelocity(velocity, getStepDt());
	}

	[[nodiscard]]
	sf::Vector3f getConstraint() const {
		return { constraintCenter.x, constraintCenter.y, constraintRadius };
	}

	[[nodiscard]]
	float getStepDt() const {
		return frameDt / static_cast<float>(subSteps);
	}

	[[nodiscard]]
	float getTime() const {
		return time;
	}

private:
	uint32_t					subSteps = 1;
	sf::Vector2f				gravity = { 0.0f, 4000.0f };
	float						frameDt = 0.0f;
	float						time = 0.0f;
	std::vector<VerletObject>	objects;
	sf::Vector2f				constraintCenter;
	float						constraintRadius = 100.0f;

	void applyGravity() {
		for (auto& obj : objects) {
			obj.accelerate(gravity);
		}
	}

	void updateObjects(float dt) {
		for (auto& obj : objects) {
			obj.update(dt);
		}
	}

	void applyConstraint() {
		for (auto& obj : objects) {
			const sf::Vector2f v = constraintCenter - obj.position;
			const float dist = sqrt((v.x * v.x) + (v.y * v.y));
			if (dist > (constraintRadius - obj.objectRadius)) {
				const sf::Vector2f n = v / dist;
				obj.position = constraintCenter - n * (constraintRadius - obj.objectRadius);
			}
		}
	}

	void solveCollisions() {
		const float responseCoef = 0.75f;
		const uint64_t objectCount = getObjectCount();

		for (uint64_t i{ 0 }; i < objectCount; ++i) {
			VerletObject& object1 = objects[i];
			for (uint64_t j{ i + 1 }; j < objectCount; ++j) {
				VerletObject& object2 = objects[j];
				const sf::Vector2f v = object1.position - object2.position;
				const float distance2 = v.x * v.x + v.y * v.y;
				const float minDistance = object1.objectRadius + object2.objectRadius;
				if (distance2 < minDistance * minDistance) {
					const float distance = sqrt(distance2);
					const sf::Vector2f n = v / distance;
					const float massRatio1 = object1.objectRadius / (object1.objectRadius + object2.objectRadius);
					const float massRatio2 = object2.objectRadius / (object1.objectRadius + object2.objectRadius);
					const float delta = 0.5f * responseCoef * (distance - minDistance);
					object1.position -= n * (massRatio2 * delta);
					object2.position += n * (massRatio1 * delta);
				}
			}
		}

	}
};