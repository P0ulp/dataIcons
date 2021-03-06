#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    //string ouput = ofSystem("/users/sallary/Library/Android/sdk/platform-tools/adb shell getevent -l -c 5 /dev/input/event2");
    
    //1440 x 2560 taille de la captation sur pixel XL
    //454 x 807 taille de la planche en pixel à 72dpi
    //595 x 842 taille A4 en pixel à 72dpi
    
    ofSetVerticalSync(true);
    
    bg.load("goldBG.jpeg");
    
    debug = true;
    savePdf = false;
    layer = 0;
    
    float ratioX =2560.0/wPlanche;
    float ratioY =1440.0/hPlanche;
    
    ofVec3f point = ofVec3f(-1,-1,-1);
    
    // this is our buffer to store the text data
    ofBuffer buffer = ofBufferFromFile("dataSC.txt");
    
    if(buffer.size()) {
        for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
            
            string line = *it;
            
            // copy the line to draw later
            // make sure its not a empty line
            if(line.empty() == false) {
                if(line.find("ABS_MT_POSITION_X") != std::string::npos){
                    if(point.y != -1){
                        point.x = points[points.size()-1].x;
                    }
                    else{
                        point.y = hPlanche-ofHexToInt(line.substr(52 ,8))/ratioY;
                    }
                    point.z = ofToFloat(line.substr(4 ,12));
                }
                else if(line.find("ABS_MT_POSITION_Y") != std::string::npos){
                    if(point.x != -1){
                        point.y = points[points.size()-1].y;
                    }
                    else{
                        point.x = ofHexToInt(line.substr(52 ,8))/ratioX;
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
    
    size.set("size",2,0,3);
    rows.set("rows",3,0,100);
    lines.set("lines",3,0,100);
    lineDistanceBG.set("BG line lentgh (m)","");
    lineDistanceFG.set("FG line lentgh (m)","");
    
    ofSetCircleResolution(50);
    
    gui.setup();
    gui.add(size);
    gui.add(rows);
    gui.add(lines);
    gui.add(lineDistanceBG);
    gui.add(lineDistanceFG);
    
}

//--------------------------------------------------------------
void ofApp::update(){
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(255,255,255);
    bg.draw(0,0,wPlanche,hPlanche);
    
    float rowSize = wPlanche/float(rows);
    float lineSize = hPlanche/float(lines);
    
    if( savePdf ){
        ofBeginSaveScreenAsPDF(ofGetTimestampString()+"Layer-"+ofToString(layer)+".pdf", false);
        layer++;
    }
    
    ofSetColor(255,255,255);
    float distanceStop = distanceBG/2.9;
    ofSetLineWidth(8);

    for(int i = 0; i < linesDatasBG.size();i++){
        ofDrawLine(linesDatasBG[i].x,linesDatasBG[i].y,linesDatasBG[i].z,linesDatasBG[i].w);
        distanceStop -= ofDist(linesDatasBG[i].x,linesDatasBG[i].y,linesDatasBG[i].z,linesDatasBG[i].w);
        if(distanceStop < 0 && savePdf){
            if(savePdf){
                ofEndSaveScreenAsPDF();
                ofBeginSaveScreenAsPDF(ofGetTimestampString()+"Layer-"+ofToString(layer)+".pdf", false);
                layer++;
            }
            distanceStop = distanceBG/2.9;
        }
    }
    
    if(savePdf){
        ofEndSaveScreenAsPDF();
        ofBeginSaveScreenAsPDF(ofGetTimestampString()+"Layer-"+ofToString(layer)+".pdf", false);
        layer++;
    }
    
    ofSetLineWidth(4);
    //ofSetColor(49, 79, 192);
    //ofSetColor(255, 0, 0);
    ofSetColor(255, 252, 0);
    for(int i=0; i<linesFG.size();i++){
        linesFG[i].draw();
        if(savePdf){
            ofEndSaveScreenAsPDF();
            if(i<linesFG.size()-1){
                ofBeginSaveScreenAsPDF(ofGetTimestampString()+"Layer-"+ofToString(layer)+".pdf", false);
                layer++;
            }
        }
    }

   
    if(savePdf){
        layer = 0;
        savePdf = false;
    }
    
    /* ------------------------------- */
    /* ---------- DEBUG GUI ---------- */
    /* ------------------------------- */
    if(debug){
        ofSetLineWidth(1);
        ofSetColor(0, 255, 0);
        lineFG.draw();
        
        ofSetColor(0, 266, 0);
        ofNoFill();
        for(int i = 0; i < circles.size();i++){
            if(circles[i].z > rowSize/2){
                ofDrawCircle(circles[i].x, circles[i].y, circles[i].z/2);
            }
        }
        ofFill();
        
        ofSetColor(255, 0, 0);
        for(int i=0; i<points.size(); i++){
            ofDrawCircle(points[i].x,points[i].y,1);
        }
        
        ofSetColor(125, 125, 0);
        for(int i = 0; i < cross.size();i++){
            ofDrawCircle(cross[i].x, cross[i].y, 5);
        }
        
        ofSetLineWidth(1);
        ofSetColor(100, 100, 100);
        for(int i = 1; i <= lines;i++){
            ofDrawLine(0,i*lineSize,wPlanche,i*lineSize);
        }
        for(int i = 1; i <= rows;i++){
            ofDrawLine(i*rowSize,0,i*rowSize,hPlanche);
        }

        ofNoFill();
        ofSetColor(0, 100, 255);
        ofDrawRectangle(0,  0, wPlanche, hPlanche);
        ofFill;
        
        gui.draw();
    }
}

//--------------------------------------------------------------
bool ofApp::compCells(ofVec2f i, ofVec2f j){
    return (i.y < j.y);
}

//--------------------------------------------------------------
void ofApp::generateDesign(){
    float rowSize = wPlanche/float(rows);
    float lineSize = hPlanche/float(lines);
    
    //reset the number of touchpoint by cells
    cells.clear();
    for(int i = 0; i < rows*lines;i++){
        cells.push_back(0);
    }
    
    //count the number of touchpoint by cells
    for(int i=0; i<points.size(); i++){
        float x = floor(points[i].x/rowSize);
        float y = floor(points[i].y/lineSize);
        cells[x+y*rows] +=1;
    }
    
    //generate a vector of cell ordered min to max points by cells
    cellsSorted.clear();
    for(int i=0; i<cells.size(); i++){
        cellsSorted.push_back(ofVec2f(i,cells[i]));
    }
    std::sort(cellsSorted.begin(),cellsSorted.end(), ofApp::compCells);
    
    //generate a line based on the ordered cells
    lineFG.clear();
    for(int i=0; i<cellsSorted.size(); i++){
        if(cellsSorted[i].y>0){
            int x = int(cellsSorted[i].x)%rows;
            int y = (cellsSorted[i].x - x)/rows;
            
            x = (rowSize/2)+x*rowSize;
            y = (lineSize/2)+y*lineSize;
            
            x = x+ofRandom(-rowSize*0.25,rowSize*0.25);
            y = y+ofRandom(-lineSize*0.25,lineSize*0.25);
            
            lineFG.addVertex(ofPoint(x,y));
        }
    }
    
    distanceFG = lineFG.getLengthAtIndex(lineFG.size()-1);
    lineDistanceFG.set(ofToString((distanceFG*ratioSketchPlanche)/1000));
    
    linesFG.clear();
    float distanceStop = distanceFG/2.9;
    ofPolyline tempLine;
    tempLine.addVertex(lineFG[0]);
    for(int i=1;i<lineFG.size();i++){
        tempLine.addVertex(lineFG[i]);
        distanceStop -= ofDist(lineFG[i].x,lineFG[i].y, lineFG[i-1].x, lineFG[i-1].y);
        if(distanceStop < 0 || i == lineFG.size()-1){
            linesFG.push_back(tempLine);
            distanceStop = distanceFG/2.9;
            tempLine.clear();
            tempLine.addVertex(lineFG[i]);
        }
    }
    
    //generate circles
    circles.clear();
    for(int i = 0; i < lines;i++){
        for(int j = 0; j < rows;j++){
            if(cells[i*rows+j] > 0){
                float x = j * rowSize;
                float y = i * lineSize;
                float circleSize = cells[i*rows+j]*size;
                circles.push_back(ofVec3f(x+rowSize/2,y+lineSize/2,circleSize));
            }
        }
    }
    
    //
    cross.clear();
    linesDatasBG.clear();
    distanceBG = 0;
    for(int i=0; i < circles.size();i++){
        if(circles[i].z > rowSize/2){
            for(int j=0; j < circles.size();j++){
                if(i!=j && circles[j].z > rowSize/2){
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
                        
                        if(paX < wPlanche-rowSize && paY < hPlanche-lineSize && paX > rowSize && paY > lineSize){
                           cross.push_back(ofVec2f(paX, paY));
                        
                            /* ---- SC ---- */
                            float pYbis = max(paY-ofRandom(lineSize*4,lineSize*8), ofRandom(lineSize*0.5,lineSize*2));
                            float pXbis = paX+ofRandom(-rowSize*0.25,rowSize*0.25);
                        
                            /* ---- YT / FB ---- */
                            /*float pXbis = max(paX-ofRandom(rowSize*6,rowSize*12), ofRandom(rowSize*0.5,rowSize*2));
                            float pYbis = paY+ofRandom(-lineSize*0.85,lineSize*0.85);*/
                        
                            float a = (paY - pYbis) / (paX - pXbis);
                            float b = paY - a* paX;
                            
                            /* ---- YT / FB ---- */
                            /*float pX = min(paX+ofRandom(rowSize*6,rowSize*12), wPlanche-ofRandom(rowSize*0.5,rowSize*2));
                            float pY = a*pX+b;*/
                            
                            /* ---- SC ---- */
                            float pY = min(paY+ofRandom(lineSize*4,lineSize*8), hPlanche-ofRandom(lineSize*0.5,lineSize*2));
                            float pX = (pY-b)/a;
                        
                            linesDatasBG.push_back(ofVec4f(pXbis,pYbis, pX, pY));
                            distanceBG += ofDist(pXbis,pYbis, pX, pY);
                        }
                    }
                }
            }
        }
    }
    lineDistanceBG.set(ofToString((distanceBG*ratioSketchPlanche)/1000));
    
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
    else if(key == 's' ){
        savePdf = true;
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

