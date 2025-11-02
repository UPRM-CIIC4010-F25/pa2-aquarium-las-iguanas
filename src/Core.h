#include <iostream>
#include <memory>
#include <utility>
#include <cmath>
#include <algorithm>
#include "ofMain.h"


class AwaitFrames {
public:
	AwaitFrames(int frames) : m_frames(frames), m_counter(0) {}
	bool tick() {
		if (m_counter < m_frames) {
			++m_counter;
			return false;
		}
		m_counter = 0; // Reset counter after reaching the target
		return true;
	}
private:
	int m_frames;
	int m_counter;
};

class GameSprite {
public:
    GameSprite(const std::string& imagePath, int width, int height) {
        if (!m_image.load(imagePath)) {
            std::cerr << "Failed to load image: " << imagePath << std::endl;
        }
        m_image.resize(width, height);
        m_flippedImage = m_image;
        m_flippedImage.mirror(false, true); // Mirror horizontally
    }

    void draw(float x, float y) const {
        if (m_flipped) {
            m_flippedImage.draw(x, y);
        } else {
            m_image.draw(x, y);
        }
    }

    void setFlipped(bool flipped) { m_flipped = flipped; }

private:
    ofImage m_image;
    ofImage m_flippedImage;
    bool m_flipped = false;
};



class Creature {
protected:
    Creature(float x, float y, int speed, float collisionRadius, int value,
             std::shared_ptr<GameSprite> sprite)
    : m_x(x)
    , m_y(y)
    , m_dx(0)
    , m_dy(0)
    , m_speed(speed)
    , m_width(0)
    , m_height(0)
    , m_collisionRadius(collisionRadius)
    , m_value(value)
    , m_sprite(std::move(sprite)) {}

    float m_x = 0.0f;
    float m_y = 0.0f;
    float m_dx = 0.0f;
    float m_dy = 0.0f;
    int m_speed = 0;
    float m_width = 0.0f;
    float m_height = 0.0f;
    float m_collisionRadius = 0.0f;
    int m_value = 0;
    std::shared_ptr<GameSprite> m_sprite;

public:
    virtual ~Creature() = default;
    virtual void move() = 0;
    virtual void draw() const = 0;

    virtual float getCollisionRadius() const { return m_collisionRadius; }
    virtual void setCollisionRadius(float radius) { m_collisionRadius = radius; }

    float getX() const { return m_x; }
    float getY() const { return m_y; }
    int getSpeed() const { return m_speed; }
    void setSpeed(int speed) { m_speed = speed; }
    void setFlipped(bool flipped) {
        if (m_sprite) {
            m_sprite->setFlipped(flipped);
        }
    }
    void setSprite(std::shared_ptr<GameSprite> sprite) { m_sprite = std::move(sprite); }
    int getValue() const { return m_value; }

    void setBounds(int w, int h);
    void normalize();
    void bounce();
    void handleWallCollision();
};

// GameEvents
enum class GameEventType {
    NONE,
    COLLISION,
    CREATURE_ADDED,
    CREATURE_REMOVED,
    GAME_OVER,
    GAME_EXIT,
    NEW_LEVEL,
};

class GameEvent {
    public:
    GameEventType type;
    std::shared_ptr<Creature> creatureA;
    std::shared_ptr<Creature> creatureB; // For collision events
    GameEvent() : type(GameEventType::NONE), creatureA(nullptr), creatureB(nullptr) {}
    GameEvent(GameEventType t, std::shared_ptr<Creature> a , std::shared_ptr<Creature> b){
        type = t;
        creatureA = a;
        creatureB = b;
    }
    
    // Additional methods can be added here
    bool isCollisionEvent() const { return type == GameEventType::COLLISION; }
    bool isCreatureAddedEvent() const { return type == GameEventType::CREATURE_ADDED; }
    bool isCreatureRemovedEvent() const { return type == GameEventType::CREATURE_REMOVED; }
    bool isGameOver() const { return type == GameEventType::GAME_OVER; }
    bool isGameExit() const { return type == GameEventType::GAME_EXIT; }
    bool isNoneEvent() const { return type == GameEventType::NONE; }
    
    // i want a printable representation of the event, with the creature descriptions if available
    void print() const;
};




bool checkCollision(std::shared_ptr<Creature> a, std::shared_ptr<Creature> b);


class GameLevel {
public:
    GameLevel(int levelNumber) : m_levelNumber(levelNumber) {}
    virtual ~GameLevel() = default;
    int getLevelNumber() const { return m_levelNumber; }
    virtual bool isCompleted() = 0;

protected:
    int m_levelNumber;
    
};




class GameScene {
    public:
        virtual string GetName() = 0;
        virtual void Update() = 0;
        virtual void Draw() = 0;
        virtual ~GameScene() = default;

};

enum class GameSceneKind {
    GAME_INTRO,
    AQUARIUM_GAME,
    GAME_OVER
};

string GameSceneKindToString(GameSceneKind t);

class GameIntroScene : public GameScene {
    public:
        GameIntroScene(string name, std::shared_ptr<GameSprite> banner)
        : m_name(name), m_banner(std::move(banner)){};
        string GetName() override {return this->m_name;}
        void Update() override;
        void Draw() override;
    private:
        string m_name;
        std::shared_ptr<GameSprite> m_banner;
};

class GameOverScene : public GameScene {
    public:
        GameOverScene(string name, std::shared_ptr<GameSprite> banner)
        : m_name(name), m_banner(std::move(banner)){};
        string GetName() override {return this->m_name;}
        void Update() override;
        void Draw() override;
    private:
        string m_name;
        std::shared_ptr<GameSprite> m_banner;
};


class GameSceneManager {
    public:
        void Transition(string name);
        void AddScene(std::shared_ptr<GameScene> newScene);
        bool HasScenes(){return m_scenes.size() > 0; }
        std::shared_ptr<GameScene> GetScene(string name);
        std::shared_ptr<GameScene> GetActiveScene();
        
        // support the functionality
        string GetActiveSceneName();
        void UpdateActiveScene();
        void DrawActiveScene();

    private:
        std::vector<std::shared_ptr<GameScene>> m_scenes;
        std::shared_ptr<GameScene> m_active_scene;

};