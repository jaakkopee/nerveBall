#include "nerveBall.h"
#include <iostream>

int nerveBall::helper::random(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

float nerveBall::helper::distance(sf::Vector2f p1, sf::Vector2f p2)
{
    return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}

float nerveBall::helper::distanceSquared(sf::Vector2f p1, sf::Vector2f p2)
{
    return pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2);
}

float nerveBall::helper::angle(sf::Vector2f p1, sf::Vector2f p2)
{
    return atan2(p2.y - p1.y, p2.x - p1.x);
}

float nerveBall::helper::length(sf::Vector2f v)
{
    return sqrt(pow(v.x, 2) + pow(v.y, 2));
}

float nerveBall::helper::lengthSquared(sf::Vector2f v)
{
    return pow(v.x, 2) + pow(v.y, 2);
}

sf::Vector2f nerveBall::helper::normalize(sf::Vector2f v)
{
    return v / length(v);
}

sf::Vector2f nerveBall::helper::vector(float length, float angle)
{
    return sf::Vector2f(length * cos(angle), length * sin(angle));
}

float nerveBall::helper::dot(sf::Vector2f v1, sf::Vector2f v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

float nerveBall::helper::cross(sf::Vector2f v1, sf::Vector2f v2)
{
    return v1.x * v2.y - v1.y * v2.x;
}

sf::Vector2f nerveBall::helper::project(sf::Vector2f v1, sf::Vector2f v2)
{
    float temps =  dot(v1, v2) / pow(length(v2), 2);
    return sf::Vector2f(temps * v2.x, temps * v2.y);
}

sf::Vector2f nerveBall::helper::reject(sf::Vector2f v1, sf::Vector2f v2)
{
    return v1 - project(v1, v2);
}

sf::Vector2f nerveBall::helper::reflect(sf::Vector2f v1, sf::Vector2f v2)
{
    sf::Vector2f temps = project(v1, v2);
    return sf::Vector2f(2 * temps.x - v1.x, 2 * temps.y - v1.y);
}

sf::Vector2f nerveBall::helper::rotate(sf::Vector2f v, float angle)
{
    return sf::Vector2f(v.x * cos(angle) - v.y * sin(angle), v.x * sin(angle) + v.y * cos(angle));
}

float nerveBall::helper::angle(sf::Vector2f v)
{
    return atan2(v.y, v.x);
}

float nerveBall::helper::angleDegrees(sf::Vector2f v)
{
    return atan2(v.y, v.x) * 180 / M_PI;
}

float nerveBall::helper::angleBetween(sf::Vector2f v1, sf::Vector2f v2)
{
    return acos(dot(v1, v2) / (length(v1) * length(v2)));
}

float nerveBall::helper::angleBetweenDegrees(sf::Vector2f v1, sf::Vector2f v2)
{
    return acos(dot(v1, v2) / (length(v1) * length(v2))) * 180 / M_PI;
}

nerveBall::Ball::Ball(sf::Vector2f position, sf::Vector2f velocity, double radius, sf::Color color)
{
    this->position = position;
    this->velocity = velocity;
    this->radius = radius;
    this->color = color;
    this->direction = 0;
    this->neuralActivation = 0;
    this->neuralActivationThreshold = 0.3;
    this->addToNeuralActivation = 0.0000000000001;
    this->shape = sf::CircleShape(this->radius);
}

double nerveBall::Ball::getNeuralActivation()
{
    this->neuralActivation += this->addToNeuralActivation;
    return this->neuralActivation;
}

void nerveBall::Ball::setNeuralActivation(double neuralActivation)
{
    this->neuralActivation = neuralActivation;
}

void nerveBall::Ball::update()
{
    if(this->neuralActivation > this->neuralActivationThreshold)
    {
        this->neuralActivation = 0;
        //this->divide();
    }
    double direction = helper::angle(this->velocity);
    direction += nerveBall::helper::random(-0.1, 0.1);
    double speed = helper::length(this->velocity);
    speed += nerveBall::helper::random(-0.1, 0.1);
    this->velocity = helper::vector(speed, direction);

    this->position += this->velocity;
    if(this->position.x < 0)
    {
        this->position.x = 0;
        this->velocity.x *= -1;
        this->direction = helper::angle(this->velocity);
    }
    if(this->position.x > 800)
    {
        this->position.x = 800;
        this->velocity.x *= -1;
        this->direction = helper::angle(this->velocity);
    }
    if(this->position.y < 0)
    {
        this->position.y = 0;
        this->velocity.y *= -1;
        this->direction = helper::angle(this->velocity);
    }
    if(this->position.y > 600)
    {
        this->position.y = 600;
        this->velocity.y *= -1;
        this->direction = helper::angle(this->velocity);
    }


    this->shape.setPosition(this->position);
}

void nerveBall::Ball::draw(sf::RenderWindow& window)
{
    this->shape.setRadius(this->radius);
    this->shape.setFillColor(this->color);
    this->shape.setOrigin(this->radius, this->radius);
    window.draw(this->shape);
}

void nerveBall::Ball::divide()
{
    //some logic to divide the ball
}

void nerveBall::Ball::setVelocity(sf::Vector2f velocity)
{
    this->velocity = velocity;
    this->direction = helper::angle(this->velocity);
}

void nerveBall::Ball::setPosition(sf::Vector2f position)
{
    this->position = position;
}

void nerveBall::Ball::setColor(sf::Color color)
{
    this->color = color;
}

sf::Vector2f nerveBall::Ball::getPosition()
{
    return this->position;
}

sf::Vector2f nerveBall::Ball::getVelocity()
{
    return this->velocity;
}

sf::Color nerveBall::Ball::getColor()
{
    return this->color;
}

void nerveBall::Ball::setDirection(double direction)
{
    this->direction = direction;
    this->velocity = helper::vector(helper::length(this->velocity), this->direction);
}

double nerveBall::Ball::getDirection()
{
    return this->direction;
}

bool nerveBall::Ball::isClicked(sf::Vector2f mousePosition)
{
    return helper::distance(this->position, mousePosition) < this->radius;
}

bool nerveBall::Ball::isColliding(Ball& other)
{
    return helper::distance(this->position, other.position) < this->radius + other.radius;
}

bool nerveBall::Ball::isColliding(std::vector<Ball>& others)
{
    for(int i = 0; i < others.size(); i++)
    {
        if(this->isColliding(others[i]))
        {
            return true;
        }
    }
    return false;
}

nerveBall::Connection::Connection(Ball* ball_from, Ball* ball_to, double weight)
{
    this->ball_from = ball_from;
    this->ball_to = ball_to;
    this->weight = weight;
}

void nerveBall::Connection::update()
{
    this->ball_to->setNeuralActivation(this->ball_to->neuralActivation + this->ball_from->getNeuralActivation() * this->weight);
}

nerveBall::BallNetwork::BallNetwork()
{
    this->balls = std::vector<Ball*>();
    this->connections = std::vector<Connection*>();
}

void nerveBall::BallNetwork::update()
{
    for(int i = 0; i < this->connections.size(); i++)
    {
        this->connections[i]->update();
    }
    for(int i = 0; i < this->balls.size(); i++)
    {
        this->balls[i]->update();
    }
}

void nerveBall::BallNetwork::draw(sf::RenderWindow& window)
{
    for(int i = 0; i < this->balls.size(); i++)
    {
        this->balls[i]->draw(window);
    }
}

void nerveBall::BallNetwork::addBall(Ball* ball)
{
    this->balls.push_back(ball);
}

nerveBall::Connection* nerveBall::BallNetwork::addConnection(Ball* ball_from, Ball* ball_to, double weight)
{
    this->connections.push_back(new Connection(ball_from, ball_to, weight));
    ball_from->connections.push_back(this->connections.back());
    ball_to->connections.push_back(this->connections.back());
    return this->connections.back();
}

void nerveBall::BallNetwork::removeBall(Ball* ball)
{
    for(int i = 0; i < this->balls.size(); i++)
    {
        if(this->balls[i] == ball)
        {
            this->balls.erase(this->balls.begin() + i);
            break;
        }
    }
    for(int i = 0; i < this->connections.size(); i++)
    {
        if(this->connections[i]->ball_from == ball || this->connections[i]->ball_to == ball)
        {
            this->connections.erase(this->connections.begin() + i);
            i--;
        }
    }
}

void nerveBall::BallNetwork::backPropagate()
{
    double target = 1.0;
    double* activations = new double[this->balls.size()];
    double learningRate = 0.001;
    double* errors = new double[this->balls.size()];
    for(int i = 0; i < this->balls.size(); i++)
    {
        activations[i] = this->balls[i]->getNeuralActivation();
    }
    for(int i = 0; i < this->balls.size(); i++)
    {
        errors[i] = activations[i] * (1 - activations[i]) * (target - activations[i]);
    }
    for(int i = 0; i < this->connections.size(); i++)
    {
        this->connections[i]->weight += learningRate * errors[i] * activations[i];
    }
    delete[] activations; 
    delete[] errors;

}

int main()
{
    srand(time(NULL));
    sf::RenderWindow window(sf::VideoMode(800, 600), "Nerve Ball");
    window.setFramerateLimit(60);
    nerveBall::BallNetwork network = nerveBall::BallNetwork();
    for(int i = 0; i < 10; i++)
    {
        network.addBall(new nerveBall::Ball(sf::Vector2f(nerveBall::helper::random(0, 800), nerveBall::helper::random(0, 600)), sf::Vector2f(nerveBall::helper::random(-5, 5), nerveBall::helper::random(-5, 5)), 10, sf::Color::White));
    }
    for(int i = 0; i < network.balls.size(); i++)
    {
        for(int j = 0; j < network.balls.size(); j++)
        {
            if(i != j)
            {
                network.addConnection(network.balls[i], network.balls[j], 0.01);
            }
        }
    }
    while(window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
            {
                window.close();
            }
            if(event.type == sf::Event::MouseButtonPressed)
            {
                if(event.mouseButton.button == sf::Mouse::Left)
                {
                    for(int i = 0; i < network.balls.size(); i++)
                    {
                        if(network.balls[i]->isClicked(sf::Vector2f(event.mouseButton.x, event.mouseButton.y)))
                        {
                            network.removeBall(network.balls[i]);
                            network.addBall(new nerveBall::Ball(sf::Vector2f(event.mouseButton.x, event.mouseButton.y), sf::Vector2f(nerveBall::helper::random(-5, 5), nerveBall::helper::random(-5, 5)), 5, sf::Color::Blue));
                            for (int j = 0; j < network.balls.size(); j++)
                            {
                                if (i != j)
                                {
                                    network.addConnection(network.balls[i], network.balls[j], 0.01);
                                }
                            }

                        }
                    }
                }
            }
        }
        //check for collisions
        for(int i = 0; i < network.balls.size(); i++)
        {
            for(int j = 0; j < network.balls.size(); j++)
            {
                if(i != j)
                {
                    if(network.balls[i]->isColliding(*network.balls[j]))
                    {
                        //a collision changes the direction of both balls to be the exact opposite of the other
                        sf::Vector2f v1 = network.balls[i]->getVelocity();
                        sf::Vector2f v2 = network.balls[j]->getVelocity();
                        sf::Vector2f p1 = network.balls[i]->getPosition();
                        sf::Vector2f p2 = network.balls[j]->getPosition();
                        double direction1 = nerveBall::helper::angle(v1);
                        double direction2 = nerveBall::helper::angle(v2);
                        double distance = nerveBall::helper::distance(p1, p2);
                        double angle = nerveBall::helper::angle(p1, p2);
                        double newDirection1 = angle + M_PI;
                        double newDirection2 = angle;
                        network.balls[i]->setDirection(newDirection1);
                        network.balls[j]->setDirection(newDirection2);

                    }
                }
            }
        }
        window.clear();
        network.update();
        network.backPropagate();
        network.draw(window);
        window.display();
    }
    return 0;
}