#pragma once

#include "ofMain.h"
#include "Tile.h"
#include "CheckPoint.h"
#include "PotentialTile.hpp"
#include "CheckPoint.h"

#include "TileCutter.hpp"
#include "UniqueTileButton.h"

class ofApp : public ofBaseApp{

public:
    void setup();
    void reset();
    void doFirstMove();
    
    void setSourceFromString(string map);
    void setNeightborInfo();
    
    void advance();
    void updateBoardFromMove(CheckPoint * point);
    void validateBoard();
    
    void update();
    void draw();
    
    void resetOutput();
    void revertToCheckPoint(CheckPoint * point);

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void drawOnSourceImage(int x, int y, int newID);
    
    int tileSize;
    vector<Tile> sourceTiles;
    
    bool needFirstMove;
    bool needToGetNeighborInfo;
    
    //#define SOURCE_COLS 39
    // #define SOURCE_ROWS 24
    int sourceCols, sourceRows;
    //int sourceImage[SOURCE_COLS][SOURCE_ROWS];
    vector< vector<int> > sourceImage;
    
    #define OUTPUT_COLS 40
    #define OUTPUT_ROWS 30
    PotentialTile outputImage[OUTPUT_COLS][OUTPUT_ROWS];
    
    CheckPoint * rootMove;
    CheckPoint * curMove;
    
    TileCutter tileCutter;
    bool useTileCutter;
    
    vector<UniqueTileButton> uniqueButtons;
    int curSelectedButton;
    
    bool autoPlay;
    bool fastForward;
    
		
};
