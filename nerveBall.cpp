#include "nerveBall.h"
#include <iostream>
#include <chrono>
#include <thread>

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
    this->neuralActivationThreshold = 0.8;
    this->addToNeuralActivation = 0.00005;
    this->shape = sf::CircleShape(this->radius);
}

double nerveBall::Ball::getNeuralActivation()
{
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
    //max=1.0
    //min=0.0
    if (this->neuralActivation < 0)
    {
        this->neuralActivation = 0;
    }
    if (this->neuralActivation > 1)
    {
        this->neuralActivation = 1;
    }

    double direction = helper::angle(this->velocity);
    direction += this->neuralActivation;
    double speed = helper::length(this->velocity);
    speed += this->neuralActivation/100;
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
    return helper::distance(this->position, mousePosition) < this->radius + 3;
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
    this->ball_to->neuralActivation += this->ball_from->addToNeuralActivation * this->weight;
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

nerveBall::Ball* nerveBall::BallNetwork::addBall(Ball* ball)
{
    this->balls.push_back(ball);
    return this->balls.back();
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

void nerveBall::lifeCountThread(Player* player, sf::RenderWindow& window)
{
    while(nerveBall::gameIsOn)
    {
        std::this_thread::sleep_for(std::chrono::seconds(60));
        //decrement the lives of the player
        player->updateLifeCount(player, player->getLives() - 1, window);
    }
}

void nerveBall::Player::updateLifeCount(Player* player, int lives, sf::RenderWindow& window)
{
    player->setLives(lives);
    if (player->getLives() == 0)
    {
        nerveBall::gameOver(player, window);
        return;
    }
    player->update();
    player->draw(window);
}

void nerveBall::gameOver(Player* player, sf::RenderWindow& window)
{
    nerveBall::gameIsOn = false;
}


void nerveBall::BallNetwork::divideBall(Ball* ball, Player* player, sf::RenderWindow& window)
{
    if (ball->radius < 5)
    {
        this->removeBall(ball);
        player->setLives(player->getLives() + 1);
        player->score += (int)(3000.0/ball->radius);
        player->updateLifeCount(player, player->getLives(), window);
        
        return;
    }
    
    player->score += (int)(3000.0/ball->radius);
    player->update();
    player->draw(window);

    sf::Vector2f position = ball->getPosition();
    sf::Vector2f velocity = ball->getVelocity();
    double radius = ball->radius-3;
    sf::Color color = ball->getColor();
    color.r -= 50;
    color.g -= 50;
    this->removeBall(ball);
    nerveBall::Ball* newball1 = this->addBall(new Ball(position, velocity, radius, color));
    nerveBall::Ball* newball2 = this->addBall(new Ball(position, velocity, radius, color));
    //connect the new balls to the old balls
    //first ball
    for(int i = 0; i < this->balls.size(); i++)
    {
        if(this->balls[i] != newball1)
        {
            this->addConnection(this->balls[i], newball1, 0.001);
        }
    }
    //second ball
    for(int i = 0; i < this->balls.size(); i++)
    {
        if(this->balls[i] != newball2)
        {
            this->addConnection(this->balls[i], newball2, 0.001);
        }
    }
}

void nerveBall::BallNetwork::backPropagate()
{
    double target = 0.001;
    double* activations = new double[this->balls.size()];
    double learningRate = 0.0001;
    for(int i = 0; i < this->balls.size(); i++)
    {
        activations[i] = this->balls[i]->getNeuralActivation();
    }
    for(int i = 0; i < this->balls.size(); i++)
    {
        double error = target - activations[i];
        double delta = error * learningRate;
        for (int j = 0; j < this->balls[i]->connections.size(); j++)
        {
            this->balls[i]->connections[j]->weight += delta;
        }
    }
    delete[] activations;

}

nerveBall::BallNetwork::~BallNetwork()
{
    for(int i = 0; i < this->balls.size(); i++)
    {
        delete this->balls[i];
    }
    for(int i = 0; i < this->connections.size(); i++)
    {
        delete this->connections[i];
    }
}

nerveBall::Player::Player()
{
    this->score = 0;
    this->lives = 5;
    this->font = sf::Font();
    this->font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    this->scoreText = sf::Text("Score: " + std::to_string(this->score), this->font, 20);
    this->scoreText.setPosition(10, 10);
    this->livesText = sf::Text("Time: " + std::to_string(this->lives) + " minutes", this->font, 20);
    this->livesText.setPosition(10, 40);
}

nerveBall::Player::~Player()
{

}

void nerveBall::Player::update()
{
    this->scoreText.setString("Score: " + std::to_string(this->score));
    this->livesText.setString("Time: " + std::to_string(this->lives) + " minutes");
}

void nerveBall::Player::draw(sf::RenderWindow& window)
{
    window.draw(this->scoreText);
    window.draw(this->livesText);
}

void nerveBall::Player::setScore(int score)
{
    this->score = score;
}

int nerveBall::Player::getScore()
{
    return this->score;
}

void nerveBall::Player::setLives(int lives)
{
    this->lives = lives;
}

int nerveBall::Player::getLives()
{
    return this->lives;
}


int main()
{
    srand(time(NULL));
    sf::RenderWindow window(sf::VideoMode(800, 600), "Nerve Ball");
    window.setFramerateLimit(60);
    nerveBall::BallNetwork network = nerveBall::BallNetwork();
    nerveBall::Player *player1 = new nerveBall::Player();
    for(int i = 0; i < 3; i++)
    {
        network.addBall(new nerveBall::Ball(sf::Vector2f(nerveBall::helper::random(0, 800), nerveBall::helper::random(0, 600)), sf::Vector2f(nerveBall::helper::random(-5, 5), nerveBall::helper::random(-5, 5)), 15, sf::Color::White));
    }
    for(int i = 0; i < network.balls.size(); i++)
    {
        for(int j = 0; j < network.balls.size(); j++)
        {
            if(i != j)
            {
                network.addConnection(network.balls[i], network.balls[j], 0.001);
            }
        }
    }
    nerveBall::gameIsOn = true;
    std::thread lifeCount(nerveBall::lifeCountThread, player1, std::ref(window));
    lifeCount.detach();

    while(window.isOpen())
    {
        if (nerveBall::gameIsOn == false)
        {
            sf::Font font;
            font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
            sf::Text gameOverText = sf::Text("Game Over", font, 50);
            sf::Text scoreText = sf::Text("Score: " + std::to_string(player1->getScore()), font, 30);
            gameOverText.setPosition(200, 200);
            scoreText.setPosition(200, 300);
            window.clear();
            window.draw(gameOverText);
            window.draw(scoreText);
            window.display();
            std::this_thread::sleep_for(std::chrono::seconds(5));
            window.close();
            exit(0);
        }

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
                           network.divideBall(network.balls[i], player1, window);
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
        player1->update();
        player1->draw(window);
        window.display();
    }
    delete player1;
    return 0;
}