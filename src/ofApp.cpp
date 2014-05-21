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

	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetFrameRate(30);
	ofSetVerticalSync(true);

	ofEnableDepthTest(); //glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	ofDisableArbTex(); //needed for textures to work normalised coords
	//glDepthFunc(GL_LEQUAL);
	shader.load("GroundFromSpace");
	earth_day.loadImage("earth_day4k.jpg");
	earth_night.loadImage("earth_night4k.jpg");

	sphere = genSphere(10.0, 4);
	sphere2 = genSphere(10.25, 4);

	cam.setGlobalPosition(0.0000, -22.1860, 12.6686);
	cam.setOrientation(ofVec3f(60, 0, 0));
	cam.setFarClip(1000);
	cam.setVFlip(true);

	quadric = gluNewQuadric();
	gluQuadricTexture(quadric, GL_TRUE);
	gluQuadricNormals(quadric, GLU_SMOOTH);

	//prepare vars for shader
	cameraDistance = cam.getPosition().distance(ofVec3f(0,0,0));
	lightDistance = sqrt(200*200*3);

	m_nSamples = 3;
	m_Kr = 0.0025f;
	m_Kr4PI = m_Kr*4.0f*PI;
	m_Km = 0.0010f;
	m_Km4PI = m_Km*4.0f*PI;
	m_ESun = 20.0;

	m_g = -0.990f;
	m_fExposure = 2.0f;

	m_fInnerRadius = 10.0f;
	m_fOuterRadius = 10.25f;
	m_fScale = 1 /(m_fOuterRadius - m_fInnerRadius);

	m_fWavelength[0] = 0.650f;              // 650 nm for red
	m_fWavelength[1] = 0.570f;              // 570 nm for green
	m_fWavelength[2] = 0.475f;              // 475 nm for blue
	m_fWavelength4[0] = powf(m_fWavelength[0], 4.0f);
	m_fWavelength4[1] = powf(m_fWavelength[1], 4.0f);
	m_fWavelength4[2] = powf(m_fWavelength[2], 4.0f);

	m_fRayleighScaleDepth = 0.25f;
	m_fMieScaleDepth = 0.1f;
	//m_pbOpticalDepth.MakeOpticalDepthBuffer(m_fInnerRadius, m_fOuterRadius, m_fRayleighScaleDepth, m_fMieScaleDepth);
}

//--------------------------------------------------------------
void ofApp::update(){
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofVec3f LightDirection = ofVec3f(0,0,1000)/ 1000;
	cam.begin();
	shader.begin();
	shader.setUniform3f("v3CameraPos:",cam.getPosition()[0], cam.getPosition()[1], cam.getPosition()[2]);
	//ofLogError() << cam.getPosition();
	shader.setUniform3f("v3LightPos", LightDirection.x, LightDirection.y, LightDirection.z);//200.0/lightDistance, 200.0/lightDistance, 200.0/lightDistance);
	//ofLogError() << 200.0/lightDistance;
	shader.setUniform3f("v3InvWavelength", 1/m_fWavelength4[0], 1/m_fWavelength4[1], 1/m_fWavelength4[2]);
	//shader.setUniform3f("m_fWavelength", 0.650, 0.570, 0.475);
	shader.setUniform1f("fCameraHeight", cam.getPosition().distance(ofVec3f(0,0,0)));
	shader.setUniform1f("fCameraHeight2", cam.getPosition().distanceSquared(ofVec3f(0,0,0)));
	//ofLogError() << cameraDistance*cameraDistance;
	shader.setUniform1f("fInnerRadius", m_fInnerRadius);
	//ofLogError() << (m_fInnerRadius);
	shader.setUniform1f("fInnerRadius2", m_fInnerRadius*m_fInnerRadius);
	shader.setUniform1f("fOuterRadius", m_fOuterRadius);
	//ofLogError() << (m_fOuterRadius);
	shader.setUniform1f("fOuterRadius2", m_fOuterRadius*m_fOuterRadius);
	//ofLogError() << (m_fOuterRadius*m_fOuterRadius);
	shader.setUniform1f("fKrESun", m_Kr*m_ESun);
	//ofLogError() << (m_Kr*m_ESun);
	shader.setUniform1f("fKmESun", m_Km*m_ESun);
	//ofLogError() << (m_Km*m_ESun);
	shader.setUniform1f("fKr4PI", m_Kr4PI);
	//ofLogError() << (m_Kr4PI);
	shader.setUniform1f("fKm4PI", m_Km4PI);
	//ofLogError() << (m_Km4PI);
	shader.setUniform1f("fScale",1.0f / (m_fOuterRadius - m_fInnerRadius));
	//ofLogError() << (1.0 / (m_fOuterRadius - m_fInnerRadius));
	shader.setUniform1f("fScaleDepth", m_fRayleighScaleDepth);
	//ofLogError() << (m_fRayleighScaleDepth);
	shader.setUniform1f("fScaleOverScaleDepth",(1.0 / (m_fOuterRadius - m_fInnerRadius))/ m_fRayleighScaleDepth);
	shader.setUniform1f("g", m_g);
	shader.setUniform1f("g2", m_g*m_g);
	shader.setUniform1f("s2Test", 0);
	//ofLogError() << ((1.0 / (m_fOuterRadius - m_fInnerRadius))/ m_fRayleighScaleDepth);
	//shader.setSampler("earthday",earth_day.getTextureReference());
	shader.setUniformTexture("s2Tex1",earth_day.getTextureReference(), 0);
	//shader.setUniform1f("fExposure", m_fExposure);
	shader.setUniformTexture("s2Tex2",earth_night.getTextureReference(), 1);
	//shader.setUniform1i("nSamples", m_nSamples);
	//shader.setUniform1f("fSamples", (float)m_nSamples);
	//ofRotateY(ofGetFrameNum());
	//gluSphere(quadric, 10, 100, 100);
	glEnable(GL_CULL_FACE);     // Cull back facing polygons
	glCullFace(GL_BACK);
	ofSetColor(255);
	//earth_day.getTextureReference().bind();
	//earth_night.getTextureReference().bind();
	gluSphere(quadric, m_fInnerRadius, 100, 50);
	//sphere.draw();
	shader.end();
	//earth_day.getTextureReference().unbind();
	//earth_night.getTextureReference().unbind();
	//glCullFace(GL_FRONT);
	//ofSetColor(255);
	//sphere2.draw();
	cam.end();
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
