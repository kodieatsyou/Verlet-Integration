#pragma once
#include "verlet.hpp"

class Renderer
{
public:
	explicit
		Renderer(sf::RenderTarget& target)
			: m_target{ target }
		{}

	void render(const VerletSolver& solver) const {
		//Render Constraint
		sf::Vector3f constraint = solver.getConstraint();
		sf::CircleShape constraintBG{constraint.z};
		constraintBG.setOrigin(constraint.z, constraint.z);
		constraintBG.setFillColor(sf::Color::Black);
		constraintBG.setPosition(constraint.x, constraint.y);
		constraintBG.setPointCount(128);
		m_target.draw(constraintBG);


		// Render Objects in solver
		sf::CircleShape circle{1.0f};
		const auto& objects = solver.getObjects();
		for (const auto& obj : objects) {
			circle.setPosition(obj.position);
			circle.setOrigin(obj.objectRadius, obj.objectRadius);
			//circle.setScale(obj.objectRadius, obj.objectRadius);
			circle.setRadius(obj.objectRadius);
			circle.setFillColor(obj.objectColor);
			m_target.draw(circle);
		}
	}
private:
	sf::RenderTarget& m_target;
};