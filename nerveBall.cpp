#include "nerveBall.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <mutex>

//hello
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
    this->neuralActivation = 0.2;
    this->neuralActivationThreshold = 0.3;
    this->addToNeuralActivation = 0.00000002;
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

double nerveBall::scaleActivation(double activation)
{
    //a downward slope
    if (activation < 0)
    {
        return activation*activation*-1;
    }
    //an upward slope
    if (activation > 0)
    {
        return activation*activation;
    }
    return activation;
}

double nerveBall::scaleActivationSigmoid(double activation)
{
    return (2.0/(1+exp(-activation)))-1.0;
}


void nerveBall::Ball::update()
{
    double activation = 0;
    for(int i = 0; i < this->connections.size(); i++)
    {
        if (this->connections[i]->ball_to == this){
            activation += this->connections[i]->getNeuralActivation();
            activation += this->addToNeuralActivation;
        }

    }
    activation = nerveBall::scaleActivationSigmoid(activation);
    this->neuralActivation += activation;

    if (this->neuralActivation >= this->neuralActivationThreshold || this->neuralActivation <= -this->neuralActivationThreshold)
    {
        this->neuralActivation = 0;
    }
    

    double direction = helper::angle(this->velocity);
    direction += this->neuralActivation/62;
    double speed = helper::length(this->velocity);
    speed += this->neuralActivation/200;
    this->velocity = helper::vector(speed, direction);
    if (speed < 2){
        this->velocity = helper::vector(2, direction);}

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

double nerveBall::Ball::getRadius()
{
    return this->radius;
}

bool nerveBall::Ball::isClicked(sf::Vector2f mousePosition)
{
    return helper::distance(this->position, mousePosition) < this->radius + 5;
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

nerveBall::Connection::~Connection()
{

}

void nerveBall::Connection::update()
{
    this->ball_to->neuralActivation += this->ball_from->neuralActivation * this->weight;
}

double nerveBall::Connection::getNeuralActivation()
{
    return this->ball_from->neuralActivation * this->weight;
}

nerveBall::BallNetwork::BallNetwork()
{
    this->balls = std::vector<Ball*>();
    this->connections = std::vector<Connection*>();
    this->soundOutput = nerveBall::soundOutput;
    this->synths = std::vector<Synth*>();
    //create some synths
    Note note1 = Note("C3", 0.06, 1);
    Note note1_ = Note("C4", 0.06, 1);
    Note note1__ = Note("F#3", 0.06, 1);
    Note note2 = Note("E3", 0.06, 1);
    Note note2_ = Note("E4", 0.06, 1);
    Note note2__ = Note("A3", 0.06, 1);
    Note note3 = Note("G3", 0.06, 1);
    Note note3_ = Note("G4", 0.06, 1);
    Note note3__ = Note("C4", 0.06, 1);
    std::vector<Note> notes;

    notes.push_back(note1);
    notes.push_back(note1_);
    notes.push_back(note1__);
    notes.push_back(note2);
    notes.push_back(note2_);
    notes.push_back(note2__);
    notes.push_back(note3);
    notes.push_back(note3_);
    notes.push_back(note3__);
    Sequence sequence1 = Sequence(notes);

    Note note4 = Note("E4", 0.06, 1);
    Note note4_ = Note("E5", 0.06, 1);
    Note note4__ = Note("A4", 0.06, 1);
    Note note5 = Note("G4", 0.06, 1);
    Note note5_ = Note("G5", 0.06, 1);
    Note note5__ = Note("C5", 0.06, 1);
    Note note6 = Note("B4", 0.06, 1);
    Note note6_ = Note("B5", 0.06, 1);
    Note note6__ = Note("E5", 0.06, 1);
    std::vector<Note> notes2;

    notes2.push_back(note4);
    notes2.push_back(note4_);
    notes2.push_back(note4__);
    notes2.push_back(note5);
    notes2.push_back(note5_);
    notes2.push_back(note5__);
    notes2.push_back(note6);
    notes2.push_back(note6_);
    notes2.push_back(note6__);
    
    Sequence sequence2 = Sequence(notes2);
    Note note7 = Note("G4", 0.06, 1);
    Note note7_ = Note("G5", 0.06, 1);
    Note note7__ = Note("C5", 0.06, 1);
    Note note8 = Note("B4", 0.06, 1);
    Note note8_ = Note("B5", 0.06, 1);
    Note note8__ = Note("E5", 0.06, 1);
    Note note9 = Note("D5", 0.06, 1);
    Note note9_ = Note("D6", 0.06, 1);
    Note note9__ = Note("G5", 0.06, 1);
    std::vector<Note> notes3;

    notes3.push_back(note7);
    notes3.push_back(note7_);
    notes3.push_back(note7__);
    notes3.push_back(note8);
    notes3.push_back(note8_);
    notes3.push_back(note8__);
    notes3.push_back(note9);
    notes3.push_back(note9_);
    notes3.push_back(note9__);

    Sequence sequence3 = Sequence(notes3);
    Note note10 = Note("B4", 0.06, 1);
    Note note10_ = Note("B5", 0.06, 1);
    Note note10__ = Note("E5", 0.06, 1);
    Note note11 = Note("D5", 0.06, 1);
    Note note11_ = Note("D6", 0.06, 1);
    Note note11__ = Note("G5", 0.06, 1);
    Note note12 = Note("F5", 0.06, 1);
    Note note12_ = Note("F6", 0.06, 1);
    Note note12__ = Note("A5", 0.06, 1);
    std::vector<Note> notes4;

    notes4.push_back(note10);
    notes4.push_back(note10_);
    notes4.push_back(note10__);
    notes4.push_back(note11);
    notes4.push_back(note11_);
    notes4.push_back(note11__);
    notes4.push_back(note12);
    notes4.push_back(note12_);
    notes4.push_back(note12__);
    
    Sequence sequence4 = Sequence(notes4);
    Note note13 = Note("D5", 0.06, 1);
    Note note13_ = Note("D6", 0.06, 1);
    Note note13__ = Note("G5", 0.06, 1);
    Note note14 = Note("F5", 0.06, 1);
    Note note14_ = Note("F6", 0.06, 1);
    Note note14__ = Note("A5", 0.06, 1);
    Note note15 = Note("A5", 0.06, 1);
    Note note15_ = Note("A6", 0.06, 1);
    Note note15__ = Note("D6", 0.06, 1);
    std::vector<Note> notes5;
    
    notes5.push_back(note13);
    notes5.push_back(note13_);
    notes5.push_back(note13__);
    notes5.push_back(note14);
    notes5.push_back(note14_);
    notes5.push_back(note14__);
    notes5.push_back(note15);
    notes5.push_back(note15_);
    notes5.push_back(note15__);
    
    Sequence sequence5 = Sequence(notes5);


    this->synths.push_back(new Synth(sequence1, this->soundOutput));
    this->synths.push_back(new Synth(sequence2, this->soundOutput));
    this->synths.push_back(new Synth(sequence3, this->soundOutput));
    this->synths.push_back(new Synth(sequence4, this->soundOutput));
    this->synths.push_back(new Synth(sequence5, this->soundOutput));
    //set volumes to zero
    for(int i = 0; i < this->synths.size(); i++)
    {
        this->synths[i]->setVolume(0);
    }
}

void nerveBall::BallNetwork::update()
{
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
    //handle last ball
    if (this->balls.size() == 1 && this->balls[0]->radius < 5)
    {
        this->balls.erase(this->balls.begin());
        nerveBall::gameOver();
        return;
    }

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

void nerveBall::Player::updateLifeCount(Player* player, double lives, sf::RenderWindow& window)
{
    player->setLives(lives);
    if (player->getLives() <= 0)
    {
        nerveBall::gameOver();
        return;
    }
    player->update();
    player->draw(window);
}

void nerveBall::gameOver()
{
    nerveBall::gameIsOn = false;
}


void nerveBall::BallNetwork::divideBall(Ball* ball, Player* player, sf::RenderWindow& window)
{

    //play a sound
    if (ball->radius == 15)
    {
        std::thread thread = std::thread(&nerveBall::BallNetwork::playSound, this, 0);
        thread.detach();
    }
    if (ball->radius == 12)
    {
        std::thread thread = std::thread(&nerveBall::BallNetwork::playSound, this, 1);
        thread.detach();
    }
    if (ball->radius == 9)
    {
        std::thread thread = std::thread(&nerveBall::BallNetwork::playSound, this, 2);
        thread.detach();
    }
    if (ball->radius == 6)
    {
        std::thread thread = std::thread(&nerveBall::BallNetwork::playSound, this, 3);
        thread.detach();
    }
    if (ball->radius == 3)
    {
        std::thread thread = std::thread(&nerveBall::BallNetwork::playSound, this, 4);
        thread.detach();
    }

    if (ball->radius < 5)
    {
        this->removeBall(ball);
        player->setLives(player->getLives() + 1);
        int addToScore = (int)(helper::length(ball->velocity)*3000.0/ball->radius);
        player->setScore(player->getScore() + addToScore);
        nerveBall::lastScore=addToScore;
        player->updateLifeCount(player, player->getLives(), window);
        
        return;
    }

    int addToScore2 = (int)(helper::length(ball->velocity)*3000.0/ball->radius);
    player->setScore(player->getScore() + addToScore2);
    nerveBall::lastScore=addToScore2;
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
            //this->addConnection(newball1, this->balls[i], -0.001);
        }
    }
    //second ball
    for(int i = 0; i < this->balls.size(); i++)
    {
        if(this->balls[i] != newball2)
        {
            this->addConnection(this->balls[i], newball2, 0.001);
            //this->addConnection(newball2, this->balls[i], -0.001);
        }
    }
    //connect the new balls to each other
    this->addConnection(newball1, newball2, 0.001);
    //this->addConnection(newball2, newball1, -0.001);
}

std::mutex mtx01;
void nerveBall::BallNetwork::playSound(int index)
{
    mtx01.lock();
    this->synths[index]->play();
    mtx01.unlock();
}

void nerveBall::BallNetwork::backPropagate()
{
    double target = 0.0;
    double* activations = new double[this->balls.size()];
    double learningRate = 0.007;
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

void nerveBall::BallNetwork::setWeigths(double weight)
{
    for(int i = 0; i < this->connections.size(); i++)
    {
        this->connections[i]->weight = weight;
    }
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
    this->font.loadFromFile("./shagade.ttf");
    this->scoreText = sf::Text("Score: " + std::to_string(this->score), this->font, 20);
    this->scoreText.setPosition(10, 10);
    this->livesText = sf::Text("Time: " + std::to_string(this->lives) + " minutes", this->font, 20);
    this->livesText.setPosition(10, 40);
    this->lastScoreText = sf::Text("Last split: " + std::to_string(nerveBall::lastScore), this->font, 20);
    this->lastScoreText.setPosition(10, 70);
    this->lettersOfName = std::vector<std::string>(3);
    this->lettersOfName[0] = "A";
    this->lettersOfName[1] = "A";
    this->lettersOfName[2] = "A";
    this->letterOfNameIndex = 0;
}

nerveBall::Player::~Player()
{

}

void nerveBall::Player::update()
{
    this->scoreText.setString("Score: " + std::to_string(this->score));
    this->livesText.setString("Time: " + std::to_string(this->lives) + " min");
    this->lastScoreText.setString("Last split: " + std::to_string(nerveBall::lastScore));
}

void nerveBall::Player::draw(sf::RenderWindow& window)
{
    window.draw(this->scoreText);
    window.draw(this->livesText);
    window.draw(this->lastScoreText);
}

void nerveBall::Player::setScore(int score)
{
    this->score = score;
}

int nerveBall::Player::getScore()
{
    return this->score;
}

void nerveBall::Player::setLives(double lives)
{
    this->lives = lives;
}

double nerveBall::Player::getLives()
{
    return this->lives;
}

void nerveBall::Player::setLetterOfName(char letter)
{
    this->lettersOfName[this->letterOfNameIndex] = letter;
}

std::vector<std::string> nerveBall::Player::getLettersOfName()
{
    return this->lettersOfName;
}

void nerveBall::Player::setLetterOfNameIndex(int letterOfNameIndex)
{
    this->letterOfNameIndex = letterOfNameIndex;
}

int nerveBall::Player::getLetterOfNameIndex()
{
    return this->letterOfNameIndex;
}

void nerveBall::slowDown(Ball* ball)
{
    double length = helper::length(ball->getVelocity());
    if (length > 0.1)
    {
        ball->setVelocity(ball->getVelocity() * 0.9f);
    }
}

void nerveBall::speedUp(Ball* ball)
{
    double length = helper::length(ball->getVelocity());
    if (length < 36)
    {
        ball->setVelocity(ball->getVelocity() * 1.1f);
    }
}

void nerveBall::slowDown(BallNetwork& network)
{
    for(int i = 0; i < network.balls.size(); i++)
    {
        nerveBall::slowDown(network.balls[i]);
    }
}

void nerveBall::speedUp(BallNetwork& network)
{
    for(int i = 0; i < network.balls.size(); i++)
    {
        nerveBall::speedUp(network.balls[i]);
    }
}




int main()
{
    srand(time(NULL));

    sf::RenderWindow window(sf::VideoMode(800, 600), "Nerve Ball", sf::Style::Close);
    window.setFramerateLimit(60);
    nerveBall::soundOutput = SoundOutputSFML();
    nerveBall::BallNetwork network = nerveBall::BallNetwork();
    nerveBall::Player *player1 = new nerveBall::Player();
    nerveBall::lastScore = 0;
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
                //network.addConnection(network.balls[j], network.balls[i], -0.001);
            }
        }
    }
    nerveBall::gameIsOn = true;
    nerveBall::introIsOn = true;

    std::thread audioThread;
    std::thread lifeCount;
    SoundFilePlayer SFPlayer = SoundFilePlayer();
    SFPlayer.load("./05_rend01.wav");
    audioThread = std::thread(&SoundFilePlayer::play, &SFPlayer);
    while(window.isOpen())
    {
        sf::Event introevent;
        int r = 0;
        int g = 0;
        int b = 0;
        double text0phase = 0;
        double text1phase = 0;
        double text2phase = 0;
        while (nerveBall::introIsOn)
        {
            while(window.pollEvent(introevent))
            {
                if(introevent.type == sf::Event::Closed)
                {
                    SFPlayer.stop();
                    if (audioThread.joinable()) {
                        audioThread.join(); // Stop the thread
                    }
                    window.close();
                    exit(0);
                }
                if(introevent.type == sf::Event::KeyPressed)
                {
                    if(introevent.key.code == sf::Keyboard::Space)
                    {
                        SFPlayer.stop();
                        if (audioThread.joinable()) {
                            audioThread.join(); // Stop the thread
                        }
                        nerveBall::introIsOn = false;
                        lifeCount = std::thread(&nerveBall::lifeCountThread, player1, std::ref(window));
                        lifeCount.detach();
                    }
                }
            }
            window.clear(sf::Color(0, 0, 0));
            r+=2;
            g+=3;
            b+=4;
            text0phase+=0.03;
            text1phase+=0.033;
            text2phase+=0.0333;
            double modulatorPhase0 = text0phase * 2;
            double modulatorPhase1 = text1phase * 0.8;
            double modulatorPhase2 = text2phase * 1.2;
            if (text0phase > 360)
            {
                text0phase = 0;
            }
            if (text1phase > 360)
            {
                text1phase = 0;
            }
            if (text2phase > 360)
            {
                text2phase = 0;
            }
            //position is sin(phase)
            int text0y = 150 + 20*sin(text0phase + 10*sin(modulatorPhase0));
            int text1y = 250 + 20*sin(text1phase + 10*sin(modulatorPhase1));
            int text2y = 300 + 20*sin(text2phase + 10*sin(modulatorPhase2));


            sf::Color color0 = sf::Color(r%255, g%255, b%255);
            sf::Color color1 = sf::Color(255-r%255, 255-g%255, 255-b%255);
            sf::Color textcolor = sf::Color(255-r%30, 255-g%60, 255-b%255);
            sf::Color textcolor2 = sf::Color(255-r%160, 255-g%130, 255-b%255);
            sf::Color textcolor3 = sf::Color(255-r%200, 255-g%160, 255-b%255);
            sf::Color circleColor = sf::Color(r%100, g%100, b%255);
            int circleRadius = 10 + 100*sin(text0phase);
            sf::RectangleShape rect0 = sf::RectangleShape(sf::Vector2f(400, 300));
            rect0.setFillColor(color0);
            rect0.setPosition(0, 0);
            sf::RectangleShape rect1 = sf::RectangleShape(sf::Vector2f(400, 300));
            rect1.setFillColor(color1);
            rect1.setPosition(400, 0);
            sf::CircleShape circle = sf::CircleShape(circleRadius);
            circle.setFillColor(circleColor);
            circle.setPosition(400 - circle.getRadius(), 300 - circle.getRadius());
            window.draw(rect0);
            window.draw(rect1);
            window.draw(circle);

            sf::Font font;

            font.loadFromFile("./shagade.ttf");
            sf::Text introText = sf::Text("Nerve Ball", font, 60);
            introText.setFillColor(textcolor);
            introText.setPosition(400 - introText.getLocalBounds().width/2, text0y);
            introText.setScale(1 + 0.1*sin(text0phase), 1 + 0.1*sin(text0phase));
            window.draw(introText);
            sf::Text introText2 = sf::Text("Nerve Wrecking, ball splitting action!", font, 30);
            introText2.setFillColor(textcolor2);
            introText2.setPosition(400 - introText2.getLocalBounds().width/2, text1y);
            introText2.setScale(1 + 0.1*sin(text1phase), 1 + 0.1*sin(text1phase));
            window.draw(introText2);
            sf::Text introText3 = sf::Text("Do my balls get on your nerves?", font, 30);
            introText3.setFillColor(textcolor3);
            introText3.setPosition(400 - introText3.getLocalBounds().width/2, text2y);
            introText3.setScale(1 + 0.1*sin(text2phase), 1 + 0.1*sin(text2phase));
            window.draw(introText3);
            window.display();
        }

        if (nerveBall::gameIsOn == false)
        {
            //game over lane
            //count the balls and substract from score
            double minusScore = 0;
            for(int i = 0; i < network.balls.size(); i++)
            {
                double radius = network.balls[i]->getRadius();
                double length = nerveBall::helper::length(network.balls[i]->getVelocity());
                minusScore+=length*3000.0/radius;
            }
            double timeBonus = player1->getLives() * 700;
            int totalScore = player1->getScore() - (int)minusScore + (int)timeBonus;

            sf::Font font;
            font.loadFromFile("./shagade.ttf");
            sf::Text gameOverText = sf::Text("Game Over", font, 50);
            sf::Text scoreText = sf::Text("Score: " + std::to_string(player1->getScore()), font, 30);
            sf::Text minusScoreText = sf::Text("Ball penalty: " + std::to_string((int)minusScore), font, 30);
            sf::Text timeBonusText = sf::Text("Time bonus: " + std::to_string((int)timeBonus), font, 30);
            sf::Text totalScoreText = sf::Text("Total: " + std::to_string(totalScore), font, 30);
            sf::Text firstLetterText = sf::Text(player1->getLettersOfName()[0], font, 30);
            sf::Text secondLetterText = sf::Text(player1->getLettersOfName()[1], font, 30);
            sf::Text thirdLetterText = sf::Text(player1->getLettersOfName()[2], font, 30);
            sf::Text firstHiScoreText = sf::Text("A", font, 30);
            sf::Text secondHiScoreText = sf::Text("A", font, 30);
            sf::Text thirdHiScoreText = sf::Text("A", font, 30);

            //read scores from file
            std::ifstream file;
            file.open("scores.txt");
            std::string line;
            std::vector<std::string> names = std::vector<std::string>();
            std::vector<int> scores = std::vector<int>();
            while(std::getline(file, line))
            {
                std::string name = line.substr(0, 3);
                int score = std::stoi(line.substr(4, line.length()));
                names.push_back(name);
                scores.push_back(score);
            }
            file.close();

            //bubble sort
            for(int i = 0; i < scores.size(); i++)
            {
                for(int j = 0; j < scores.size() - 1; j++)
                {
                    if (scores[j] < scores[j+1])
                    {
                        int temp = scores[j];
                        scores[j] = scores[j+1];
                        scores[j+1] = temp;
                        std::string temp2 = names[j];
                        names[j] = names[j+1];
                        names[j+1] = temp2;
                    }
                }
            }


            firstHiScoreText.setString(names[0] + " " + std::to_string(scores[0]));
            secondHiScoreText.setString(names[1] + " " + std::to_string(scores[1]));
            thirdHiScoreText.setString(names[2] + " " + std::to_string(scores[2]));

            gameOverText.setPosition(400 - gameOverText.getLocalBounds().width/2, 25);
            scoreText.setPosition(400 - scoreText.getLocalBounds().width/2, 100);
            minusScoreText.setPosition(400 - minusScoreText.getLocalBounds().width/2, 150);
            timeBonusText.setPosition(400 - timeBonusText.getLocalBounds().width/2, 200);
            totalScoreText.setPosition(400 - totalScoreText.getLocalBounds().width/2, 250);
            firstLetterText.setPosition(400 - firstLetterText.getLocalBounds().width/2 - 30, 300);
            secondLetterText.setPosition(400 - secondLetterText.getLocalBounds().width/2, 300);
            thirdLetterText.setPosition(400 - thirdLetterText.getLocalBounds().width/2 + 30, 300);
            firstHiScoreText.setPosition(400 - firstHiScoreText.getLocalBounds().width/2, 350);
            secondHiScoreText.setPosition(400 - secondHiScoreText.getLocalBounds().width/2, 400);
            thirdHiScoreText.setPosition(400 - thirdHiScoreText.getLocalBounds().width/2, 450);

            sf::Color gameOverColor(255, 255, 255);
            sf::Color scoreColor(255, 255, 255);
            sf::Color minusScoreColor(255, 255, 255);
            sf::Color timeBonusColor(255, 255, 255);
            sf::Color totalScoreColor(255, 255, 255);
            sf::Color firstLetterColor(255, 255, 255);
            sf::Color secondLetterColor(255, 255, 255);
            sf::Color thirdLetterColor(255, 255, 255);
            sf::Color firstHiScoreColor(255, 0 ,0);
            sf::Color secondHiScoreColor(0, 255, 0);
            sf::Color thirdHiScoreColor(0, 0, 255);


            while (window.isOpen())
            {
                sf::Event event;
                while(window.pollEvent(event))
                {
                    if(event.type == sf::Event::Closed)
                    {
                        window.close();
                        exit(0);
                    }
                    //space to change letter of name index
                    if(event.type == sf::Event::KeyPressed)
                    {
                        if(event.key.code == sf::Keyboard::Space)
                        {
                            int letterOfNameIndex = player1->getLetterOfNameIndex();
                            if (letterOfNameIndex == 0)
                            {
                                player1->setLetterOfNameIndex(1);
                            }
                            if (letterOfNameIndex == 1)
                            {
                                player1->setLetterOfNameIndex(2);
                            }
                            if (letterOfNameIndex == 2)
                            {
                                player1->setLetterOfNameIndex(0);
                            }
                        }
                    }
                    //mouse wheel to change letter
                    std::string letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
                    if(event.type == sf::Event::MouseWheelMoved)
                    {
                        if(event.mouseWheel.delta > 0)
                        {
                            int letterOfNameIndex = player1->getLetterOfNameIndex();
                            std::string letter = player1->getLettersOfName()[letterOfNameIndex];
                            int index = letters.find(letter);
                            index++;
                            if (index > 25)
                            {
                                index = 0;
                            }
                            player1->setLetterOfName(letters[index]);
                        }
                        if(event.mouseWheel.delta < 0)
                        {
                            int letterOfNameIndex = player1->getLetterOfNameIndex();
                            std::string letter = player1->getLettersOfName()[letterOfNameIndex];
                            int index = letters.find(letter);
                            index--;
                            if (index < 0)
                            {
                                index = 25;
                            }
                            player1->setLetterOfName(letters[index]);
                        }
                    }
                    //enter to save score
                    if(event.type == sf::Event::KeyPressed)
                    {
                        if(event.key.code == sf::Keyboard::Return)
                        {
                            std::string name = player1->getLettersOfName()[0] + player1->getLettersOfName()[1] + player1->getLettersOfName()[2];
                            std::ofstream file;
                            file.open("scores.txt", std::ios::app);
                            file << name << " " << totalScore << std::endl;
                            file.close();



                            //read scores from file
                            std::ifstream file2;
                            file2.open("scores.txt");
                            std::string line;
                            std::vector<std::string> names = std::vector<std::string>();
                            std::vector<int> scores = std::vector<int>();
                            while(std::getline(file2, line))
                            {
                                std::string name = line.substr(0, 3);
                                int score = std::stoi(line.substr(4, line.length()));
                                names.push_back(name);
                                scores.push_back(score);
                            }
                            file2.close();
                            //bubble sort
                            for(int i = 0; i < scores.size(); i++)
                            {
                                for(int j = 0; j < scores.size() - 1; j++)
                                {
                                    if (scores[j] < scores[j+1])
                                    {
                                        int temp = scores[j];
                                        scores[j] = scores[j+1];
                                        scores[j+1] = temp;
                                        std::string temp2 = names[j];
                                        names[j] = names[j+1];
                                        names[j+1] = temp2;
                                    }
                                }
                            }
                            //update the high scores
                            firstHiScoreText.setString(names[0] + " " + std::to_string(scores[0]));
                            secondHiScoreText.setString(names[1] + " " + std::to_string(scores[1]));
                            thirdHiScoreText.setString(names[2] + " " + std::to_string(scores[2]));
                        }
                    }
                }

                int letterOfNameIndex = player1->getLetterOfNameIndex();
                if (letterOfNameIndex == 0)
                {
                    firstLetterColor = sf::Color(255, 0, 0);
                    secondLetterColor = sf::Color(255, 255, 255);
                    thirdLetterColor = sf::Color(255, 255, 255);
                }
                if (letterOfNameIndex == 1)
                {
                    firstLetterColor = sf::Color(255, 255, 255);
                    secondLetterColor = sf::Color(255, 0, 0);
                    thirdLetterColor = sf::Color(255, 255, 255);
                }
                if (letterOfNameIndex == 2)
                {
                    firstLetterColor = sf::Color(255, 255, 255);
                    secondLetterColor = sf::Color(255, 255, 255);
                    thirdLetterColor = sf::Color(255, 0, 0);
                }

                //shift colors
                int temp = gameOverColor.r;
                if (temp == 255)
                {
                    temp = 0;
                }
                gameOverColor.r = temp + 1;
                temp = gameOverColor.g;
                if (temp == 255)
                {
                    temp = 0;
                }
                gameOverColor.g = temp + 2;
                temp = gameOverColor.b;
                if (temp == 255)
                {
                    temp = 0;
                }
                gameOverColor.b = temp + 4;

                temp = scoreColor.r;
                if (temp == 255)
                {
                    temp = 0;
                }
                scoreColor.r = temp + 2;
                temp = scoreColor.g;
                if (temp == 255)
                {
                    temp = 0;
                }
                scoreColor.g = temp + 4;
                temp = scoreColor.b;
                if (temp == 255)
                {
                    temp = 0;
                }
                scoreColor.b = temp + 8;

                temp = minusScoreColor.r;
                if (temp == 255)
                {
                    temp = 0;
                }
                minusScoreColor.r = temp + 4;
                temp = minusScoreColor.g;
                if (temp == 255)
                {
                    temp = 0;
                }
                minusScoreColor.g = temp + 8;
                temp = minusScoreColor.b;
                if (temp == 255)
                {
                    temp = 0;
                }
                minusScoreColor.b = temp + 16;

                temp = timeBonusColor.r;
                if (temp == 255)
                {
                    temp = 0;
                }
                timeBonusColor.r = temp + 8;
                temp = timeBonusColor.g;
                if (temp == 255)
                {
                    temp = 0;
                }
                timeBonusColor.g = temp + 16;
                temp = timeBonusColor.b;
                if (temp == 255)
                {
                    temp = 0;
                }
                timeBonusColor.b = temp + 32;

                temp = totalScoreColor.r;
                if (temp == 255)
                {
                    temp = 0;
                }
                totalScoreColor.r = temp + 16;
                temp = totalScoreColor.g;
                if (temp == 255)
                {
                    temp = 0;
                }
                totalScoreColor.g = temp + 32;
                temp = totalScoreColor.b;
                if (temp == 255)
                {
                    temp = 0;
                }
                totalScoreColor.b = temp + 64;

                gameOverText.setFillColor(gameOverColor);
                scoreText.setFillColor(scoreColor);
                minusScoreText.setFillColor(minusScoreColor);
                timeBonusText.setFillColor(timeBonusColor);
                totalScoreText.setFillColor(totalScoreColor);
                firstLetterText.setFillColor(firstLetterColor);
                secondLetterText.setFillColor(secondLetterColor);
                thirdLetterText.setFillColor(thirdLetterColor);
                firstHiScoreText.setFillColor(firstHiScoreColor);
                secondHiScoreText.setFillColor(secondHiScoreColor);
                thirdHiScoreText.setFillColor(thirdHiScoreColor);

                firstLetterText.setString(player1->getLettersOfName()[0]);
                secondLetterText.setString(player1->getLettersOfName()[1]);
                thirdLetterText.setString(player1->getLettersOfName()[2]);

                window.clear();
                window.draw(gameOverText);
                window.draw(scoreText);
                window.draw(minusScoreText);
                window.draw(timeBonusText);
                window.draw(totalScoreText);
                window.draw(firstLetterText);
                window.draw(secondLetterText);
                window.draw(thirdLetterText);
                window.draw(firstHiScoreText);
                window.draw(secondHiScoreText);
                window.draw(thirdHiScoreText);
                window.display();
            }
            
        }
        bool ballWasClicked;
        //handle events
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
                    ballWasClicked = false;

                    for(int i = 0; i < network.balls.size(); i++)
                    {
                        if(network.balls[i]->isClicked(sf::Vector2f(event.mouseButton.x, event.mouseButton.y)))
                        {
                            network.divideBall(network.balls[i], player1, window);
                            ballWasClicked = true;
                        }

                    }

                    if (ballWasClicked == false){
                        player1->updateLifeCount(player1, player1->getLives() - 0.05, window);
                    }
                    
                }
            }
            //detect mouse wheel
            if(event.type == sf::Event::MouseWheelMoved)
            {
                if(event.mouseWheel.delta > 0)
                {
                    nerveBall::speedUp(network);
                }
                if(event.mouseWheel.delta < 0)
                {
                    nerveBall::slowDown(network);
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