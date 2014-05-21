#pragma once

#include "ofMain.h"

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
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		ofImage 	earth_day;
		ofImage 	earth_night;
		ofMesh 		sphere;
		ofMesh		sphere2;

		ofShader 	shader;

		ofCamera	cam;
		GLUquadricObj *quadric;

		//prepare vars for shader
		float cameraDistance;// = sqrt(cam.getPosition().x*cam.getPosition().x +
				//cam.getPosition().y*cam.getPosition().y +
				//cam.getPosition().z*cam.getPosition().z);
		float lightDistance;// = sqrt(200*200*3);
		int m_nSamples;// = 2;
		float m_Kr;// = 0.0025;
		float m_Kr4PI;// = m_Kr*4.0*PI;
		float m_Km;// = 0.0015;
		float m_Km4PI; // = m_Km*4.0*PI;
		float m_ESun;// = 15.0;
		float m_g;
		float m_fExposure;// = 2.0;
		float m_fInnerRadius;// = 10.0;
		float m_fOuterRadius;// = 10.250;
		float m_fScale;
		float m_fWavelength[3];
		float m_fWavelength4[3];
		float m_fRayleighScaleDepth;// = 0.25;
		float m_fMieScaleDepth;// = 0.25;


};
