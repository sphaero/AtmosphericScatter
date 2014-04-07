#include "ofApp.h"


ofMesh genSphere( float radius, int res=12, ofPrimitiveMode mode=OF_PRIMITIVE_TRIANGLE_STRIP )
{
    ofMesh mesh;

    float doubleRes = res*2.f;
    float polarInc = PI/(res); // ringAngle
    float azimInc = TWO_PI/(doubleRes); // segAngle //

    if(mode != OF_PRIMITIVE_TRIANGLE_STRIP && mode != OF_PRIMITIVE_TRIANGLES) {
        mode = OF_PRIMITIVE_TRIANGLE_STRIP;
    }
    mesh.setMode(mode);

    ofVec3f vert;
    ofVec2f tcoord;

    for(float i = 0; i < res+1; i++) {

        float tr = sin( PI-i * polarInc );
        float ny = cos( PI-i * polarInc );

        tcoord.y = i / res;

        for(float j = 0; j <= doubleRes; j++) {

            float nx = tr * sin(j * azimInc);
            float nz = tr * cos(j * azimInc);

            tcoord.x = j / (doubleRes);

            vert.set(nx, ny, nz);
            mesh.addNormal(vert);
            vert *= radius;
            mesh.addVertex(vert);
            mesh.addTexCoord(tcoord);
        }
    }

    int nr = doubleRes+1;
    if(mode == OF_PRIMITIVE_TRIANGLES) {

        int index1, index2, index3;

        for(float iy = 0; iy < res; iy++) {
            for(float ix = 0; ix < doubleRes; ix++) {

                // first tri //
                if(iy > 0) {
                    index1 = (iy+0) * (nr) + (ix+0);
                    index2 = (iy+0) * (nr) + (ix+1);
                    index3 = (iy+1) * (nr) + (ix+0);

                    mesh.addIndex(index1);
                    mesh.addIndex(index3);
                    mesh.addIndex(index2);
                }

                if(iy < res-1 ) {
                    // second tri //
                    index1 = (iy+0) * (nr) + (ix+1);
                    index2 = (iy+1) * (nr) + (ix+1);
                    index3 = (iy+1) * (nr) + (ix+0);

                    mesh.addIndex(index1);
                    mesh.addIndex(index3);
                    mesh.addIndex(index2);

                }
            }
        }

    } else {
        for(int y = 0; y < res; y++) {
            for(int x = 0; x <= doubleRes; x++) {
                mesh.addIndex( (y)*nr + x );
                mesh.addIndex( (y+1)*nr + x );
            }
        }
    }
    return mesh;
}

//--------------------------------------------------------------
void ofApp::setup(){
	ofBackground(0,0,0);

	ofSetFrameRate(30);
	ofSetVerticalSync(true);

	ofEnableDepthTest();
	ofDisableArbTex(); //needed for textures to work normalised coords
	//glDepthFunc(GL_LEQUAL);
	shader.load("shader");
	earth_day.loadImage("earth_day4k.jpg");
	earth_night.loadImage("earth_night4k.jpg");

	sphere = genSphere(10.0, 24);
	sphere2 = genSphere(10.25, 24);

	cam.setGlobalPosition(0.0000, -22.1860, 12.6686);
	cam.setOrientation(ofVec3f(60, 0, 0));
	cam.setFarClip(1000);
	cam.setVFlip(true);
}

//--------------------------------------------------------------
void ofApp::update(){
}

//--------------------------------------------------------------
void ofApp::draw(){
	cam.begin();
	glEnable(GL_CULL_FACE);     // Cull back facing polygons
	glCullFace(GL_BACK);
	ofSetColor(255);

	shader.begin();
	//prepare vars for shader
	float cameraDistance = sqrt(cam.getPosition().x*cam.getPosition().x +
			cam.getPosition().y*cam.getPosition().y +
			cam.getPosition().z*cam.getPosition().z);
	float lightDistance = sqrt(200*200*3);
	float m_fInnerRadius = 10.0;
	float m_fOuterRadius = 10.250;
	float m_ESun = 15.0;
	float m_Km = 0.0015;
	float m_Kr = 0.0025;
	float m_Km4PI = m_Km*4.0*PI;
	float m_Kr4PI = m_Kr*4.0*PI;
	float m_fRayleighScaleDepth = 0.25;
	float fExposure = 2.0;
	int m_nSamples = 2;

	shader.setUniform3f("v3CameraPos:",cam.getPosition()[0], cam.getPosition()[1], cam.getPosition()[2]);
	ofLogError() << cam.getPosition();
	shader.setUniform3f("v3LightPos", 0.5013, 0.6447, 0.5771);//200.0/lightDistance, 200.0/lightDistance, 200.0/lightDistance);
	//ofLogError() << 200.0/lightDistance;
	shader.setUniform3f("m_fWavelength", 0.650, 0.570, 0.475);
	//#shader.setUniform1f('fCameraHeight', cameraDistance)
	shader.setUniform1f("fCameraHeight2", cameraDistance*cameraDistance);
	ofLogError() << cameraDistance*cameraDistance;
	shader.setUniform1f("fInnerRadius", m_fInnerRadius);
	ofLogError() << (m_fInnerRadius);
	//#shader.setUniform1f('fInnerRadius2', m_fInnerRadius*m_fInnerRadius)
	shader.setUniform1f("fOuterRadius", m_fOuterRadius);
	ofLogError() << (m_fOuterRadius);
	shader.setUniform1f("fOuterRadius2", m_fOuterRadius*m_fOuterRadius);
	ofLogError() << (m_fOuterRadius*m_fOuterRadius);
	shader.setUniform1f("fKrESun", m_Kr*m_ESun);
	ofLogError() << (m_Kr*m_ESun);
	shader.setUniform1f("fKmESun", m_Km*m_ESun);
	ofLogError() << (m_Km*m_ESun);
	shader.setUniform1f("fKr4PI", m_Kr4PI);
	ofLogError() << (m_Kr4PI);
	shader.setUniform1f("fKm4PI", m_Km4PI);
	ofLogError() << (m_Km4PI);
	shader.setUniform1f("fScale",1.0 / (m_fOuterRadius - m_fInnerRadius));
	ofLogError() << (1.0 / (m_fOuterRadius - m_fInnerRadius));
	shader.setUniform1f("fScaleDepth", m_fRayleighScaleDepth);
	ofLogError() << (m_fRayleighScaleDepth);
	shader.setUniform1f("fScaleOverScaleDepth",(1.0 / (m_fOuterRadius - m_fInnerRadius))/ m_fRayleighScaleDepth);
	ofLogError() << ((1.0 / (m_fOuterRadius - m_fInnerRadius))/ m_fRayleighScaleDepth);
	//shader.setSampler("earthday",earth_day.getTextureReference());
	shader.setUniformTexture("earthday",earth_day.getTextureReference(), 0);
	shader.setUniform1f("fExposure", fExposure);
	//shader.setUniformTexture("earthnight",earth_night.getTextureReference(), 1);
	shader.setUniform1i("nSamples", m_nSamples);
	shader.setUniform1f("fSamples", (float)m_nSamples);
	//ofRotateY(ofGetFrameNum());
	sphere.draw();
	shader.end();
	//earth_day.getTextureReference().unbind();
	//glCullFace(GL_FRONT);
	//ofSetColor(255);
	//sphere2.draw();
	cam.end();
	/*//glEnable(GL_CULL_FACE);     // Cull back facing polygons
	cam.begin();
	ofEnableDepthTest();
	//change origin to center
	//ofTranslate(ofGetWidth()/2, ofGetHeight()/2, ofGetFrameNum());

	//rotate sphere over time
	ofRotateY(ofGetFrameNum());

	//ofRotateX(-90); //north pole facing up

	//bind and draw texture
	earth_day.getTextureReference().bind();
	//glEnable(GL_CULL_FACE);     // Cull back facing polygons
	ofEnableDepthTest();
	glCullFace(GL_BACK);
	sphere.draw();
	earth_day.getTextureReference().unbind();

	ofSetColor(255);
	glCullFace(GL_FRONT);
	sphere2.draw();
*/	/*// draw our normals, and show that they are perpendicular to the vector from the center to the vertex
	vector<ofVec3f> n = sphere.getNormals();
	vector<ofVec3f> v = sphere.getVertices();
	float normalLength = 20.;

	if(!ofGetKeyPressed()){
		ofDisableLighting();
		ofSetColor(255,255,255,220);
		for(unsigned int i=0; i < n.size() ;i++){
			//n[i] = n[i] * -1;
			ofLine(v[i].x,v[i].y,v[i].z,
				   v[i].x+n[i].x*normalLength,v[i].y+n[i].y*normalLength,v[i].z+n[i].z*normalLength);

			ofLine(.98*v[i].x,.98*v[i].y,.98*v[i].z,
				   .98*v[i].x+n[i].x*normalLength*.2,.98*v[i].y+n[i].y*normalLength*.2,.98*v[i].z+n[i].z*normalLength*.2);
			ofLine(.98*v[i].x+n[i].x*normalLength*.2,.98*v[i].y+n[i].y*normalLength*.2,.98*v[i].z+n[i].z*normalLength*.2,
				   v[i].x+n[i].x*normalLength*.2,v[i].y+n[i].y*normalLength*.2,v[i].z+n[i].z*normalLength*.2);
		}
	}*/
	//cam.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

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
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
