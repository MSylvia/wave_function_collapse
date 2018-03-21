 #include "ofApp.h"

//tiles from https://opengameart.org/content/simple-nes-like-platformer-tiles

//--------------------------------------------------------------
void ofApp::setup(){
    autoPlay = false;
    fastForward = false;
    useTileCutter = true;
    
    tileCutter.setup("full_pic.png");
    
    rootMove = new CheckPoint(NULL);
    
    if (true){
        tileCutter.cut();
        tileCutter.setData(sourceTiles, sourceImage, sourceCols, sourceRows, tileSize);
        reset();
    }
    
}

//--------------------------------------------------------------
void ofApp::reset(){
    setNeightborInfo();
    resetOutput();
    //doFirstMove();
    
    uniqueButtons.clear();
    int spacing = 2;
    int curX = spacing;
    int curY = (sourceRows+2)*tileSize + spacing;
    for (int i=0; i<sourceTiles.size(); i++){
        UniqueTileButton button;
        if (curX + tileSize+spacing > sourceCols*tileSize ){
            curY += tileSize+spacing;
            curX = spacing;
        }
        //cout<<i<<": "<<curX<<","<<curY<<endl;
        button.setup(curX, curY, tileSize, i);
        curX += tileSize + spacing;
        uniqueButtons.push_back(button);
    }
    
    
    curSelectedButton = 0;
    
    needFirstMove = true;
}

//--------------------------------------------------------------
void ofApp::doFirstMove(){
    cout<<"do it"<<endl;
    needFirstMove = false;
    //start us off
    rootMove->prune();
    curMove = new CheckPoint(rootMove);
    curMove->move(ofRandom(OUTPUT_COLS), ofRandom(OUTPUT_ROWS), ofRandom(sourceTiles.size())) ;
    updateBoardFromMove(curMove);
}

//--------------------------------------------------------------
void ofApp::setSourceFromString(string map){
    int curX = 0;
    int curY = 0;
    for (int i=0; i<map.length(); i++){
        if (map[i] == ','){
            curX++;
        }
        else if (map[i] == '\n'){
            curX=0;
            curY++;
        }
        else {
            string numString = "x";
            numString[0] = map[i];
            
            if (map[i+1] != ','){
                numString += map[i+1];
                i++;
            }
            
            int idNum = ofToInt(numString);
            //cout<<curX<<","<<curY<<" id: "<<idNum<<endl;
            if (curX < sourceCols){    //my source image was fucked up so I'm getting rid of the last column
                sourceImage[curX][curY] = idNum;
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::setNeightborInfo(){
    needToGetNeighborInfo = false;
    cout<<"get neighbor info"<<endl;
    
    for (int i=0; i<sourceTiles.size(); i++){
        sourceTiles[i].resetNeighborInfo();
    }
    
    for (int x=0; x<sourceCols; x++){
        for (int y=0; y<sourceRows; y++){
            int id = sourceImage[x][y];
            
            //check to the north
            if (y>0){
                sourceTiles[id].noteNeighbor(0, sourceImage[x][y-1]);
            }
            
            //check to the east
            if (x<sourceCols-1){
                sourceTiles[id].noteNeighbor(1, sourceImage[x+1][y]);
            }
            
            //check to the south
            if (y<sourceRows-1){
                sourceTiles[id].noteNeighbor(2, sourceImage[x][y+1]);
            }
            
            //check to the west
            if (x>0){
                sourceTiles[id].noteNeighbor(3, sourceImage[x-1][y]);
            }
            
        }
    }
    
//    for (int i=0; i<sourceTiles.size(); i++){
//        cout<<endl;
//        cout<<"TILE "<<i<<endl;
//        string label [4] = { "North", "East", "South", "West" };
//        
//        for (int k=0; k<4; k++){
//            cout<<"  "<<label[k]<<endl;
//            for (int j=0; j<sourceTiles[i].neighbors[k].size(); j++){
//                cout<<"    "<<sourceTiles[i].neighbors[k][j].idNum<<","<<sourceTiles[i].neighbors[k][j].freq<<endl;
//            }
//        }
//        
//    }
    
}

//--------------------------------------------------------------
void ofApp::advance(){
    if (needToGetNeighborInfo){
        setNeightborInfo();
    }
    if (needFirstMove){
        doFirstMove();
        return;
    }
    
    //cout<<"advance"<<endl;
    CheckPoint * oldMove = curMove;
    curMove = new CheckPoint(oldMove);
    
    //cout<<"move "<<curMove->getDepth()<<endl;
    
    //make a list of the active potential tiels with the fewest posibilits
    int lowVal = sourceTiles.size()+1;
    for (int x=0; x<OUTPUT_COLS; x++){
        for (int y=0; y<OUTPUT_ROWS; y++){
            if (outputImage[x][y].state == STATE_ACTIVE && outputImage[x][y].potentialIDs.size() < lowVal){
                lowVal = outputImage[x][y].potentialIDs.size();
            }
        }
    }
    
    vector<PotentialTile> choices;
    for (int x=0; x<OUTPUT_COLS; x++){
        for (int y=0; y<OUTPUT_ROWS; y++){
            if (outputImage[x][y].state == STATE_ACTIVE && outputImage[x][y].potentialIDs.size() == lowVal){
                choices.push_back(outputImage[x][y]);
            }
        }
    }
    
    if (choices.size() == 0){
        cout<<"all done!"<<endl;
        autoPlay = false;
        return;
    }
    
    //select one at random
    int thisChoice = (int) ofRandom(choices.size());
    //select a tile at random
    int thisTile = (int) ofRandom(choices[thisChoice].potentialIDs.size());
    //make a move
    curMove->move( choices[thisChoice].x, choices[thisChoice].y, choices[thisChoice].potentialIDs[thisTile]);
    
}

//--------------------------------------------------------------
void ofApp::updateBoardFromMove(CheckPoint * point){
    //cout<<"update board"<<endl;
    MoveInfo move = point->thisMove;
    if (move.col == -1){
        cout<<"empty move"<<endl;
        return;
    }
    
    //set the given tiles
    outputImage[move.col][move.row].set(move.idNum);
    
    //rule out anything we need to because it previously lead to dead ends
    for (int i=0; i<point->badMoves.size(); i++){
        MoveInfo badMove = point->badMoves[i];
        outputImage[badMove.col][badMove.row].ruleOutID(badMove.idNum);
        cout<<"hey brah don't do "<<badMove.col<<","<<badMove.row<<": "<<badMove.idNum<<endl;
    }
    
    //go through the neighbors and update them
    
    //north
    if (move.row > 0){
        outputImage[move.col][move.row-1].ruleOutBasedOnNeightbor( sourceTiles[outputImage[move.col][move.row].setID], 0);
    }
    
    //east
    if (move.col < OUTPUT_COLS-1){
        outputImage[move.col+1][move.row].ruleOutBasedOnNeightbor( sourceTiles[outputImage[move.col][move.row].setID], 1);
    }
    
    //south
    if (move.row < OUTPUT_ROWS-1){
        outputImage[move.col][move.row+1].ruleOutBasedOnNeightbor( sourceTiles[outputImage[move.col][move.row].setID], 2);
    }
    
    //west
    if (move.col > 0){
        outputImage[move.col-1][move.row].ruleOutBasedOnNeightbor( sourceTiles[outputImage[move.col][move.row].setID], 3);
    }
    
    //validate
    validateBoard();
}

//--------------------------------------------------------------
//if any potential tiles have no options, this move is dirt!
void ofApp::validateBoard(){
    bool boardIsValid = true;
    for (int x=0; x<OUTPUT_COLS; x++){
        for (int y=0; y<OUTPUT_ROWS; y++){
            if (outputImage[x][y].state == STATE_ACTIVE && outputImage[x][y].potentialIDs.size() == 0){
                cout<<x<<","<<y<<" is no good!"<<endl;
                boardIsValid = false;
            }
        }
    }
    
    if (!boardIsValid){
        //autoPlay = false;
        curMove->prevPoint->ruleOutMove(curMove->thisMove);
        revertToCheckPoint(curMove->prevPoint);
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    if (useTileCutter && !tileCutter.ready){
        return;
    }
    
    if (autoPlay){
        int cycles = 1;
        if (fastForward){
            cycles = 30;
        }
        for (int i=0; i<cycles; i++){
            advance();
            updateBoardFromMove(curMove);
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if (useTileCutter && !tileCutter.ready){
        ofPushMatrix();
        ofTranslate(5, 5);
        tileCutter.draw();
        ofPopMatrix();
        return;
    }
    
    //sourc eimage
    ofSetColor(255);
    for (int x=0; x<sourceCols; x++){
        for (int y=0; y<sourceRows; y++){
            sourceTiles[ sourceImage[x][y] ].pic.draw( x*tileSize, y*tileSize);
        }
    }

    //unique tiles
    for (int i=0; i<uniqueButtons.size(); i++){
        uniqueButtons[i].draw(sourceTiles[uniqueButtons[i].idNum].pic, curSelectedButton==i);
    }
    
    //output
    ofPushMatrix();
    ofTranslate(sourceCols*tileSize+tileSize, 0);
    for (int x=0; x<OUTPUT_COLS; x++){
        for (int y=0; y<OUTPUT_ROWS; y++){
            
            ofPushMatrix();
            ofTranslate(x*tileSize, y*tileSize);
            
            ofFill();
            if (outputImage[x][y].state == STATE_INACTIVE){
                ofSetColor(50);
                ofDrawRectangle(0, 0, tileSize, tileSize);
            }
            if (outputImage[x][y].state == STATE_ACTIVE){
                ofSetColor(170);
                ofDrawRectangle(0, 0, tileSize, tileSize);
                ofSetColor(0);
                ofDrawBitmapString( ofToString(outputImage[x][y].potentialIDs.size()), tileSize*0.25, tileSize*0.75);
            }
            if (outputImage[x][y].state == STATE_SET){
                ofSetColor(255);
                sourceTiles[ outputImage[x][y].setID ].pic.draw( 0,0 );
            }
            
            ofNoFill();
            ofSetColor(0);
            ofDrawRectangle(0, 0, tileSize, tileSize);
            
            ofPopMatrix();
        }
    }
    
    ofPopMatrix();
    
    ofSetColor(0);
    ofDrawBitmapString("auto: "+ofToString(autoPlay), 10,ofGetHeight()-50);
    ofDrawBitmapString("fast: "+ofToString(fastForward), 10,ofGetHeight()-30);
    
}

//--------------------------------------------------------------
void ofApp::resetOutput(){
    for (int x=0; x<OUTPUT_COLS; x++){
        for (int y=0; y<OUTPUT_ROWS; y++){
            outputImage[x][y].reset(sourceTiles.size(), x, y);
        }
    }
}

//--------------------------------------------------------------
void ofApp::revertToCheckPoint(CheckPoint * point){
    cout<<"REVERT to "<<point->getDepth()<<endl;
    resetOutput();
    
    curMove = rootMove;
    //int steps =0;
    while(curMove != point){
        //steps++;
        //cout<<"step: "<<steps<<endl;
        updateBoardFromMove(curMove);
        curMove = curMove->nextPoint;
    }
    
    updateBoardFromMove(curMove);
    
    curMove->prune();
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    //tile cutter commands
    if (useTileCutter && !tileCutter.ready){
        if (key == OF_KEY_UP){
            tileCutter.adjustTileSize(1);
        }
        if (key == OF_KEY_DOWN){
            tileCutter.adjustTileSize(-1);
        }
        if (key == OF_KEY_RETURN){
            tileCutter.cut();
            tileCutter.setData(sourceTiles, sourceImage, sourceCols, sourceRows, tileSize);
            reset();
        }
        return;
    }
    
    if (key == ' '){
        advance();
        updateBoardFromMove(curMove);
    }
    
    if (key == 'z'){
        curMove->prevPoint->ruleOutMove(curMove->thisMove);
        revertToCheckPoint(curMove->prevPoint);
    }
    
    if (key == 'a'){
        autoPlay = !autoPlay;
    }
    if (key == 'f'){
        fastForward = !fastForward;
    }
    
    if (key == 'r'){
        resetOutput();
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if (x < sourceCols*tileSize && y <sourceRows*tileSize){
        int col = x/tileSize;
        int row = y/tileSize;
        drawOnSourceImage(col, row, curSelectedButton);
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    for (int i=0; i<uniqueButtons.size(); i++){
        if (uniqueButtons[i].checkClick(x, y)){
            curSelectedButton = i;
        }
    }
    
    if (x < sourceCols*tileSize && y <sourceRows*tileSize){
        int col = x/tileSize;
        int row = y/tileSize;
        drawOnSourceImage(col, row, curSelectedButton);
    }

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void ofApp::drawOnSourceImage(int x, int y, int newID){
    sourceImage[x][y] = newID;
    needToGetNeighborInfo = true;
    if (!needFirstMove){
        resetOutput();
        needFirstMove = true;
    }
}
