#include <SFML/Graphics.hpp>
#include <iostream>
#include "verlet.hpp"
#include "renderer.hpp"
#include "number_generator.hpp"

static sf::Color getRainbow(float t)
{
    const float r = sin(t);
    const float g = sin(t + 0.33f * 2.0f * 3.1415936f);
    const float b = sin(t + 0.66f * 2.0f * 3.1415936f);
    return { static_cast<uint8_t>(255.0f * r * r),
            static_cast<uint8_t>(255.0f * g * g),
            static_cast<uint8_t>(255.0f * b * b) };
}

int main()
{
    //Make SFML Window
    constexpr uint32_t window_width = 1000;
    constexpr uint32_t window_height = 1000;
    const uint32_t framerate = 60;
    sf::ContextSettings windowSettings;
    windowSettings.antialiasingLevel = 1;
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Test", sf::Style::Default, windowSettings);
    window.setFramerateLimit(framerate);

    //Detail solver
    VerletSolver solver;
    solver.setSubStepCount(8);
    solver.setSimulationUpdateRate(framerate);
    solver.setConstraint({ static_cast<float>(window_width) * 0.5f, static_cast<float>(window_height) * 0.5f }, 450.0f);

    //Detail renderer
    Renderer renderer{ window };

    //Detail simulation settings
    const float objectSpawnDelay = 0.025f;
    const float objectSpawnSpeed = 1000.0f;
    const sf::Vector2f objectSpawnPosition = { 500.0f, 200.0f };
    const float minObjectRadius = 5.0f;
    const float maxObjectRadius = 20.0f;
    const uint32_t maxObjectCount = 2000.0f;
    const float maxAngle = 1.0f;

    sf::Clock clock;


    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                window.close();
        }

        if (solver.getObjectCount() < maxObjectCount && clock.getElapsedTime().asSeconds() >= objectSpawnDelay) {
            clock.restart();
            auto& object = solver.addObject(objectSpawnPosition, RNGf::getRange(minObjectRadius, maxObjectRadius));
            const float time = solver.getTime();
            const float angle = maxAngle * sin(time) + 3.1415936f * 0.5f;
            solver.setObjectVelocity(object, objectSpawnSpeed * sf::Vector2f{cos(angle), sin(angle)});
            object.objectColor = getRainbow(time);

        }

        solver.update();
        window.clear(sf::Color::White);
        renderer.render(solver);
        window.display();
    }

    return 0;
}