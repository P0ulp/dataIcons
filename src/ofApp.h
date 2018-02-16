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
        vector<ofVec4f> linesDatasBG;
        vector<int> cells;
        vector<ofVec2f> cellsSorted;
        vector<ofVec2f> cross;
    
        ofPolyline lineFG;
        vector<ofPolyline> linesFG;
        ofImage bg;
    
        ofxPanel gui;
        ofParameter<float> size;
        ofParameter<int> rows;
        ofParameter<int> lines;
        ofParameter<string> lineDistanceFG;
        ofParameter<string> lineDistanceBG;
    
        bool debug;
        bool savePdf;
    
    
        float distanceBG;
        float distanceFG;
    
        float wPlanche = 807;
        float hPlanche = 454;
        double ratioSketchPlanche = 290.0/wPlanche;
    
        int layer;
        
		
};
