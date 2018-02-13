#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

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
    
        void generateDesign();
        static bool compCells(ofVec2f i, ofVec2f j);
    
    private:
        vector<ofVec3f> points;
        vector<ofVec3f> circles;
        vector<ofVec4f> lines;
        vector<int> cells;
        vector<ofVec2f> cellsSorted;
        ofxPanel gui;
        ofParameter<float> size;
        ofParameter<int> row;
        ofParameter<int> line;
    
        bool debug;
    
    
        
		
};
