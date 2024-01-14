/*
    * nerveBall.h
    * Nerve Ball is an irritating ball game
    * click on the balls to divide them to smaller balls to get points
    * the smaller the balls are the more points you get
    * the smallest ball does not divide anymore when clicked, instead it disappears and you get points and play time
*/

#include <alsa/asoundlib.h>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include <queue>

namespace nerveBall
{
    // Constants
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;
    const int BALL_RADIUS = 20;
    const int BALL_SPEED = 5;


    // Function prototypes

    namespace helper
    {
        // Returns a random number between min and max
        int random(int min, int max);

        // Returns the distance between two points
        float distance(sf::Vector2f p1, sf::Vector2f p2);

        // Returns the angle between two points
        float angle(sf::Vector2f p1, sf::Vector2f p2);

        // Returns the length of a vector
        float length(sf::Vector2f v);

        // Returns a unit vector in the direction of v
        sf::Vector2f normalize(sf::Vector2f v);

        // Returns a vector with the given length and direction
        sf::Vector2f vector(float length, float angle);

        // Returns the dot product of two vectors
        float dot(sf::Vector2f v1, sf::Vector2f v2);

        // Returns the cross product of two vectors
        float cross(sf::Vector2f v1, sf::Vector2f v2);

        // Returns the projection of v1 onto v2
        sf::Vector2f project(sf::Vector2f v1, sf::Vector2f v2);

        // Returns the rejection of v1 onto v2
        sf::Vector2f reject(sf::Vector2f v1, sf::Vector2f v2);

        // Returns the reflection of v1 across v2
        sf::Vector2f reflect(sf::Vector2f v1, sf::Vector2f v2);

        // Returns the rotation of v by angle
        sf::Vector2f rotate(sf::Vector2f v, float angle);

        // Returns the angle of v
        float angle(sf::Vector2f v);

        // Returns the angle of v in degrees
        float angleDegrees(sf::Vector2f v);

        // Returns the angle between v1 and v2
        float angleBetween(sf::Vector2f v1, sf::Vector2f v2);

        // Returns the angle between v1 and v2 in degrees
        float angleBetweenDegrees(sf::Vector2f v1, sf::Vector2f v2);

        // Returns the distance between p1 and p2
        float distance(sf::Vector2f p1, sf::Vector2f p2);

        // Returns the distance between p1 and p2 squared
        float distanceSquared(sf::Vector2f p1, sf::Vector2f p2);

        // Returns the length of v
        float length(sf::Vector2f v);

        // Returns the length of v squared
        float lengthSquared(sf::Vector2f v);

        // Returns a unit vector in the direction of v
        sf::Vector2f normalize(sf::Vector2f v);

        // Returns a vector with the given length and direction
        sf::Vector2f vector(float length, float angle);
    }

    class Connection;
    class BallNetwork;
    class Ball{
        friend class Connection;
        friend class BallNetwork;
        public:
            std::vector<Connection*> connections;
            Ball(sf::Vector2f position, sf::Vector2f velocity, double radius, sf::Color color);
            double getNeuralActivation();
            void setNeuralActivation(double neuralActivation);
            void update();
            void draw(sf::RenderWindow& window);
            void divide();
            void setVelocity(sf::Vector2f velocity);
            void setPosition(sf::Vector2f position);
            void setColor(sf::Color color);
            sf::Vector2f getPosition();
            sf::Vector2f getVelocity();
            sf::Color getColor();
            void setDirection(double direction);
            double getDirection();
            bool isClicked(sf::Vector2f mousePosition);
            bool isColliding(Ball& other);
            bool isColliding(std::vector<Ball>& others);


        private:
            sf::CircleShape shape;
            sf::Vector2f velocity;
            sf::Color color;
            sf::Vector2f position;
            double radius;
            double direction;
            double neuralActivation;
            double neuralActivationThreshold;
            double addToNeuralActivation;

    };

    class Connection {
        friend class BallNetwork;
        public:

            Connection(Ball* ball_from, Ball* ball_to, double weight);

            void update();

        private:
            Ball* ball_from;
            Ball* ball_to;
            double weight;
    };

    class Player;

    class BallNetwork{
        public:
            std::vector<Ball*> balls;
            std::vector<Connection*> connections;
            BallNetwork();
            ~BallNetwork();
            void update();
            void draw(sf::RenderWindow& window);
            Ball* addBall(Ball* ball);
            Connection* addConnection(Ball* ball_from, Ball* ball_to, double weight);
            void removeBall(Ball* ball);
            void divideBall(Ball* ball, Player* player, sf::RenderWindow& window);
            void backPropagate();

    };

    class Player {
        friend class BallNetwork;
        public:
            Player();
            ~Player();
            void update();
            void draw(sf::RenderWindow& window);
            void setScore(int score);
            int getScore();
            void setLives(int lives);
            int getLives();
            void updateLifeCount(Player* player, int lives, sf::RenderWindow& window);
            

        private:
            int score;
            int lives;
            sf::Font font;
            sf::Text scoreText;
            sf::Text livesText;
    };

    bool gameIsOn;
    void lifeCountThread(Player* player, sf::RenderWindow& window);
    void gameOver(Player* player, sf::RenderWindow& window);
}
 

