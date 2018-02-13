#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    //string ouput = ofSystem("/users/sallary/Library/Android/sdk/platform-tools/adb shell getevent -l -c 5 /dev/input/event2");
    //ofLogNotice(output);
    
    debug = true;
    ofVec3f point = ofVec3f(-1,-1,-1);
    
    // this is our buffer to stroe the text data
    ofBuffer buffer = ofBufferFromFile("_data.txt");
    
    if(buffer.size()) {
        for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
            
            string line = *it;
            
            // copy the line to draw later
            // make sure its not a empty line
            if(line.empty() == false) {
                if(line.find("ABS_MT_POSITION_X") != std::string::npos){
                    if(point.x != -1){
                        point.y = points[points.size()-1].y;
                    }
                    else{
                        point.x = ofHexToInt(line.substr(52 ,8))/4.0;
                    }
                    point.z = ofToFloat(line.substr(4 ,12));
                }
                else if(line.find("ABS_MT_POSITION_Y") != std::string::npos){
                    if(point.y != -1){
                        point.x = points[points.size()-1].x;
                    }
                    else{
                        point.y = ofHexToInt(line.substr(52 ,8))/4.0;
                    }
                    point.z = ofToFloat(line.substr(4 ,12));
                }
            }
            
            if(point.y != -1 && point.x != -1){
                points.push_back(point);
                point = ofVec3f(-1,-1,-1);
            }
        }
        
    }
    
    size.set("size",2,0,5.0);
    row.set("row",3,0,100);
    line.set("line",3,0,100);
    
    ofSetCircleResolution(50);
    
    gui.setup();
    gui.add(size);
    gui.add(row);
    gui.add(line);
    
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(255, 207, 64);
    ofSetLineWidth(4);
    //1440 x 2560
    //360 x 640
    
    float rowSize = ofGetWidth()/float(row);
    float lineSize = ofGetHeight()/float(line);

    
    ofSetColor(255,255,255);
    for(int i = 0; i < lines.size();i++){
        ofDrawLine(lines[i].x,lines[i].y,lines[i].z,lines[i].w);
    }
        
    ofSetColor(49, 79, 192);
    for(int i = 0; i < circles.size();i++){
        if(circles[i].z > rowSize/2){
            ofNoFill();
        }
        if(circles[i].z < rowSize/2){
            //ofDrawCircle(circles[i].x, circles[i].y, circles[i].z/2);
        }
        ofFill();
    }
    
    for(int i=0; i<cellsSorted.size(); i++){
        if(cellsSorted[i].y>0 && i+1<cellsSorted.size()){
            int x1 = int(cellsSorted[i].x)%row;
            int y1 = (cellsSorted[i].x - x1)/row;
            int x2 = int(cellsSorted[i+1].x)%row;
            int y2 = (cellsSorted[i+1].x - x2)/row;
            ofLogNotice() << x1 << " / " <<y1 << "-" << cellsSorted[i].x << endl;

            x1 = (rowSize/2)+x1*rowSize;
            y1 = (lineSize/2)+y1*lineSize;
            x2 = (rowSize/2)+x2*rowSize;
            y2 = (lineSize/2)+y2*lineSize;
            
            ofDrawLine(x1,y1,x2,y2);
            ofLogNotice() << x1 << " / " <<y1 << "-" << cellsSorted[i].x << endl;
        }
    }
        
    ofLogNotice() << "---------- ";
    
    /* ------------------------------- */
    /* ---------- DEBUG GUI ---------- */
    /* ------------------------------- */
    if(debug){
        ofSetColor(255, 0, 0);
        for(int i=0; i<points.size(); i++){
            ofDrawCircle(points[i].x,points[i].y,1);
        }
        
        ofSetColor(200, 200, 200);
        for(int i = 1; i <= line;i++){
            ofDrawLine(0,i*lineSize,ofGetWidth(),i*lineSize);
        }
        for(int i = 1; i <= row;i++){
            ofDrawLine(i*rowSize,0,i*rowSize,ofGetHeight());
        }
        
        gui.draw();
    }
}

//--------------------------------------------------------------
bool ofApp::compCells(ofVec2f i, ofVec2f j){
    return (i.y < j.y);
}

//--------------------------------------------------------------
void ofApp::generateDesign(){
    cells.clear();
    for(int i = 0; i < row*line;i++){
        cells.push_back(0);
    }
    
    float rowSize = ofGetWidth()/float(row);
    float lineSize = ofGetHeight()/float(line);
    
    for(int i=0; i<points.size(); i++){
        float x = floor(points[i].x/rowSize);
        float y = floor(points[i].y/lineSize);
        //ofLogNotice() << "Px :" << points[i].x << " / Py :" <<points[i].y;
        //ofLogNotice() << "x :" << x << " / y :" << y << " / val :" << x+y*row;
        cells[x+y*row] +=1;
    }
    //ofLogNotice() << "______________________";
    
    cellsSorted.clear();
    for(int i=0; i<cells.size(); i++){
        cellsSorted.push_back(ofVec2f(i,cells[i]));
    }
    
    std::sort(cellsSorted.begin(),cellsSorted.end(), ofApp::compCells);
    
    circles.clear();
    for(int i = 0; i < line;i++){
        for(int j = 0; j < row;j++){
            if(cells[i*row+j] > 0){
                float x = j * rowSize;
                float y = i * lineSize;
                float circleSize = cells[i*row+j]*size;
                if(circleSize < rowSize){
                    circleSize = max(min(float(cells[i*row+j]),float(rowSize*1.5)),rowSize/4)+ofRandom(rowSize/5);
                    x = x+ofRandom(-rowSize/2,rowSize/2);
                    y = y+ofRandom(-lineSize/2,lineSize/2);
                }
                circles.push_back(ofVec3f(x+rowSize/2,y+lineSize/2,circleSize));
            }
        }
    }
    
    lines.clear();
    for(int i=0; i < circles.size();i++){
        if(circles[i].z > rowSize){
            for(int j=0; j < circles.size();j++){
                if(i!=j && circles[j].z > rowSize){
                    float d = ofDist(circles[i].x,circles[i].y,circles[j].x,circles[j].y);
                    float aR = circles[i].z/2;
                    float bR = circles[j].z/2;
                    if(d <(circles[i].z/2+circles[j].z/2) && d >= abs(bR - aR)){
                        
                        float ex = (circles[j].x - circles[i].x) / d;
                        float ey = (circles[j].y - circles[i].y) / d;
                        
                        float x = (aR * aR - bR * bR + d * d) / (2 * d);
                        float y = sqrt(aR * aR - x * x);
                        
                        float paX = circles[i].x + x * ex - y * ey;
                        float paY = circles[i].y + x * ey + y * ex;
                        float pbX = circles[i].x + x * ex + y * ey;
                        float pbY = circles[i].y + x * ey - y * ex;
                        
                        float pYbis = paY-ofRandom(paY*0.25,paY*0.75);
                        float pXbis = paX+ofRandom(-paX*0.2,paX*0.2);
                        
                        float a = (paY - pYbis) / (paX - pXbis);
                        float b = paY - a* paX;
                        
                        
                        float pY = paY+ofRandom(paY*0.1,paY*0.25);
                        float pX = (pY-b)/a;
                        
                        lines.push_back(ofVec4f(pXbis,pYbis, pX, pY));
                    }
                }
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key == ' '){
        debug = !debug;
    }
    else if(key == 'g'){
        generateDesign();
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

