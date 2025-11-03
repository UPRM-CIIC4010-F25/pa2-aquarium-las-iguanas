#include "ofApp.h"
#include "ofSoundPlayer.h"
//--------------------------------------------------------------
void ofApp::setup(){

    ofSetFrameRate(60);
    ofSetBackgroundColor(ofColor::blue);
    backgroundImage.load("background.png");
    backgroundImage.resize(ofGetWindowWidth(), ofGetWindowHeight());


    std::shared_ptr<Aquarium> myAquarium;
    std::shared_ptr<PlayerCreature> player;

    // make the game scene manager 
    gameManager = std::make_unique<GameSceneManager>();


    // first we make the intro scene 
    gameManager->AddScene(std::make_shared<GameIntroScene>(
        GameSceneKindToString(GameSceneKind::GAME_INTRO),
        std::make_shared<GameSprite>("title.png", ofGetWindowWidth(), ofGetWindowHeight())
    ));

    //AquariumSpriteManager
    spriteManager = std::make_shared<AquariumSpriteManager>();

    // Lets setup the aquarium
    myAquarium = std::make_shared<Aquarium>(ofGetWindowWidth(), ofGetWindowHeight(), spriteManager);
    player = std::make_shared<PlayerCreature>(ofGetWindowWidth()/2 - 50, ofGetWindowHeight()/2 - 50, DEFAULT_SPEED, this->spriteManager->GetSprite(AquariumCreatureType::NPCreature));
    player->setDirection(0, 0); // Initially stationary
    player->setBounds(ofGetWindowWidth() - 20, ofGetWindowHeight() - 20);


    myAquarium->addAquariumLevel(std::make_shared<Level_0>(0, 10));
    myAquarium->addAquariumLevel(std::make_shared<Level_1>(1, 15));
    myAquarium->addAquariumLevel(std::make_shared<Level_2>(2, 20));
    myAquarium->addAquariumLevel(std::make_shared<Level_3>(3, 30));
    myAquarium->addAquariumLevel(std::make_shared<Level_4>(4, 40));
    myAquarium->Repopulate(); // initial population

    if (myAquarium->getCurrentLevelNumber() == 0) {
        myAquarium->spawnFish(AquariumCreatureType::NPCreature, 6);
    } else if (myAquarium->getCurrentLevelNumber() == 1) {
        myAquarium->spawnFish(AquariumCreatureType::NPCreature, 5);
        myAquarium->spawnFish(AquariumCreatureType::FastFish, 2);
    } else if (myAquarium->getCurrentLevelNumber() == 2) {
        myAquarium->spawnFish(AquariumCreatureType::NPCreature, 4);
        myAquarium->spawnFish(AquariumCreatureType::FastFish, 3);
        myAquarium->spawnFish(AquariumCreatureType::SlowFish, 2);
    } else if (myAquarium->getCurrentLevelNumber() == 3) {
        myAquarium->spawnFish(AquariumCreatureType::NPCreature, 3);
        myAquarium->spawnFish(AquariumCreatureType::FastFish, 3);
        myAquarium->spawnFish(AquariumCreatureType::SlowFish, 2);
        myAquarium->spawnFish(AquariumCreatureType::BiggerFish, 1);
    }

    // now that we are mostly set, lets pass the player and the aquarium downstream
    gameManager->AddScene(std::make_shared<AquariumGameScene>(
        std::move(player), std::move(myAquarium), GameSceneKindToString(GameSceneKind::AQUARIUM_GAME)
    )); // player and aquarium are owned by the scene moving forward

    // Load font for game over message
    gameOverTitle.load("Verdana.ttf", 12, true, true);
    gameOverTitle.setLineHeight(34.0f);
    gameOverTitle.setLetterSpacing(1.035);


    gameManager->AddScene(std::make_shared<GameOverScene>(
        GameSceneKindToString(GameSceneKind::GAME_OVER),
        std::make_shared<GameSprite>("game-over.png", ofGetWindowWidth(), ofGetWindowHeight())
    ));

    ofSetLogLevel(OF_LOG_NOTICE); // Set default log level
    static ofSoundPlayer ambient;
    ambient.load("ambient.wav");
    ambient.setLoop(true);
    ambient.setVolume(0.3f);
    ambient.play();
}

//--------------------------------------------------------------
void ofApp::update(){
    
    ofSoundUpdate();
    if(gameManager->GetActiveSceneName() == GameSceneKindToString(GameSceneKind::GAME_OVER)){
        return; // Stop updating if game is over or exiting
    }

    if(gameManager->GetActiveSceneName() == GameSceneKindToString(GameSceneKind::AQUARIUM_GAME)){
        auto gameScene = std::static_pointer_cast<AquariumGameScene>(gameManager->GetActiveScene());
        if(gameScene->GetLastEvent() != nullptr && gameScene->GetLastEvent()->isGameOver()){
            gameManager->Transition(GameSceneKindToString(GameSceneKind::GAME_OVER));
            return;
        }
        
    }

    gameManager->UpdateActiveScene();
    


}

//--------------------------------------------------------------
void ofApp::draw(){
    backgroundImage.draw(0, 0);
    gameManager->DrawActiveScene();
    auto aquariumScene = std::static_pointer_cast<AquariumGameScene>(gameManager->GetScene(GameSceneKindToString(GameSceneKind::AQUARIUM_GAME)));
    if (aquariumScene && gameManager->GetActiveSceneName() == GameSceneKindToString(GameSceneKind::AQUARIUM_GAME)) {
        int currentLevel = aquariumScene->GetAquarium()->getCurrentLevelNumber();
        ofSetColor(255, 255, 255);
        gameOverTitle.drawString("Level: " + std::to_string(currentLevel), 20, 40);
    }
}

//--------------------------------------------------------------
void ofApp::exit(){
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (lastEvent.isGameExit()) { 
        ofLogNotice() << "Game has ended. Press ESC to exit." << std::endl;
        return; // Ignore other keys after game over
    }
    if(gameManager->GetActiveSceneName() == GameSceneKindToString(GameSceneKind::AQUARIUM_GAME)){
        auto gameScene = std::static_pointer_cast<AquariumGameScene>(gameManager->GetActiveScene());
        switch(key){
            case OF_KEY_UP:
                gameScene->GetPlayer()->setDirection(gameScene->GetPlayer()->isXDirectionActive()?gameScene->GetPlayer()->getDx():0, -1);
                break;
                case OF_KEY_DOWN:
                gameScene->GetPlayer()->setDirection(gameScene->GetPlayer()->isXDirectionActive()?gameScene->GetPlayer()->getDx():0, 1);
                break;
            case OF_KEY_LEFT:
                gameScene->GetPlayer()->setDirection(-1, gameScene->GetPlayer()->isYDirectionActive()?gameScene->GetPlayer()->getDy():0);
                gameScene->GetPlayer()->setFlipped(true);
                break;
                case OF_KEY_RIGHT:
                gameScene->GetPlayer()->setDirection(1, gameScene->GetPlayer()->isYDirectionActive()?gameScene->GetPlayer()->getDy():0);
                gameScene->GetPlayer()->setFlipped(false);
                break;
            default:
                break;
        }
    
        
    
        gameScene->GetPlayer()->move();
        return;

    }

    if(gameManager->GetActiveSceneName() == GameSceneKindToString(GameSceneKind::GAME_INTRO)){
        switch (key)
        {
        case OF_KEY_SPACE:
            gameManager->Transition(GameSceneKindToString(GameSceneKind::AQUARIUM_GAME));
            break;
        
        default:
            break;
        }
    }


    
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(gameManager->GetActiveSceneName() == GameSceneKindToString(GameSceneKind::AQUARIUM_GAME)){
        auto gameScene = std::static_pointer_cast<AquariumGameScene>(gameManager->GetActiveScene());
    if( key == OF_KEY_UP || key == OF_KEY_DOWN){
        gameScene->GetPlayer()->setDirection(gameScene->GetPlayer()->isXDirectionActive()?gameScene->GetPlayer()->getDx():0, 0);
        gameScene->GetPlayer()->move();
        return;
    }
    
    if(key == OF_KEY_LEFT || key == OF_KEY_RIGHT){
        gameScene->GetPlayer()->setDirection(0, gameScene->GetPlayer()->isYDirectionActive()?gameScene->GetPlayer()->getDy():0);
        gameScene->GetPlayer()->move();
        return;
    }

    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    backgroundImage.resize(w, h);
    auto aquariumScene = std::static_pointer_cast<AquariumGameScene>(gameManager->GetScene(GameSceneKindToString(GameSceneKind::AQUARIUM_GAME)));
    aquariumScene->GetAquarium()->setBounds(w,h);
    aquariumScene->GetPlayer()->setBounds(w - 20, h - 20);

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
