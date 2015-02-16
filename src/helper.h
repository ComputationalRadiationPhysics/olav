/**
* Copyright 2015 Alexander Matthes
*
* This file is part of OLAV.
*
* OLAV is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* OLAV is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with OLAV.
* If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#include <stdio.h>
#include <vector>
#include <math.h>
#include "Electron.h"

#define M_PI 3.14159265358979323846


//!Simply a 3D-vector with an x-, y- and z-component
typedef struct{
	float x,y,z;
}vector3;

//! This defines some methods which are generally useful all over the project.
namespace helper{


/*! Samples the user defines trajectory
@param vdt speed*Delta(t)
@param t_old The latest parameter
@param oldPosition
@param points The data points
@param tangents The tangents
@return The next position
*/
inline vector3 HermiteSampling(float vdt, float* t_old, Ogre::Vector3 oldPosition,std::vector<Ogre::Vector3> points,std::vector<Ogre::Vector3> tangents,int i,int j){
	char buffer[256];
	sprintf(buffer,"start i %i j %i",i,j);
	Ogre::LogManager::getSingletonPtr()->logMessage(buffer);
	int counter=0;
	//float s2=pow(vdt,2);
	float h0,h1,h2,h3;
	Ogre::Vector3 c;
	float d,t_low,t_high;
	t_low=*t_old;
	t_high=t_low+0.5f;
	float t;
	//bool overflow=false;
	d=1.0f;
	if (t_high>1.0) t_high=1.0f;
	float dist=points[i].distance(points[j]);

	while(abs(d-vdt)>0.0001){
	
	t=(t_low+t_high)*0.5f;

	/*if (t>1.0f){
		t=0.0f;
		*i=(*i+1)%points.size();
		j=(j+1)%points.size();
		t_high=0.5f;
		//t_low=0.0f;
		overflow=true;
	}*/
	
	

	h0=pow((1.0f-t),2)*(1.0f+2.0f*t);
	h1=t*pow((1.0f-t),2);
	h2=-pow(t,2)*(1.0f-t);
	h3=(3.0f-2.0f*t)*pow(t,2);
	
	c=h0*points[i]+h1*tangents[i].normalisedCopy()*dist+h2*tangents[j].normalisedCopy()*dist+h3*points[j];
	
	
	d=c.distance(oldPosition);
	//d=d-vdt;
	if (d<vdt) {
		t_low=t;

	}
	if (d>vdt) {
		t_high=t;
		/*if (overflow) {
			t_high=1.0f;
			*i=(*i-1+points.size())%points.size();
			j=(j-1+points.size())%points.size();
			t=1.1f;
			overflow=false;
		}*/
	}
	//d=abs(d);

	counter++;
	if (counter==1000){
		*t_old=1.1f;
		vector3 v;
		//if (!overflow){
		v.x=points[j].x;
		v.y=points[j].y;
		v.z=points[j].z;
		/*}else{
		v.x=points[*i].x;
		v.y=points[*i].y;
		v.z=points[*i].z;
		}*/
		return v;
	}
	}

	vector3 p;
	p.x=c.x;
	p.y=c.y;
	p.z=c.z;
	*t_old=t;

	sprintf(buffer,"end");
	Ogre::LogManager::getSingletonPtr()->logMessage(buffer);

	return p;
}
/*! Samples the sine curve when the undulator trajectroy is enabled.
@param vdt the speed of the electron multiplied with the passed time since the last step
@param undulatorlength the period of the trajectroy
@param amplidute the amplidute of the trajectroy
@param x_old the x-value of the latest position of the electron
@return the x-value of the electron's actual moving vector
*/
inline float SineSamplingBisection(float vdt,float undulatorlength,float amplitude,float x_old){
	
	
	float y_old=sin(x_old*2*M_PI/undulatorlength)*amplitude;
	float x_m,y_m,s;
	float x_left=vdt/sqrt(1+amplitude*amplitude*2*M_PI/undulatorlength*2*M_PI/undulatorlength)+x_old;
	float x_right=vdt+x_old;
	float delta=vdt;
	x_m=(x_left+x_right)/2;

	while(delta>0.0001){
	
		x_m=(x_left+x_right)/2;
		y_m=sin(x_m*2*M_PI/undulatorlength)*amplitude;
		s=sqrt((x_m-x_old)*(x_m-x_old)+(y_m-y_old)*(y_m-y_old));
		if (s<vdt) {
			x_left=x_m;
			delta=vdt-s;
		}
		else {
			x_right=x_m;
			delta=s-vdt;
		}
		
	}

	return x_m-x_old;
}

/*!Takes the distance between two wavefronts and calculates a colour representing the corresponding frequency
@param lambda the distance between two wavefronts
@return a vector v containg the rgb-color representation of the frequency.  v[0]=red, v[1]=green, v[2]=blue
*/ 
inline std::vector<float> halffrequencyToColour(float lambda){
		
	std::vector<float> rgb;
	float red,green,blue;
		//Start colour computation
			float V=1.0f;
			float S=1.0f;
			//float H=360.0;
					
					
			float H=15066.9657f*pow(0.990506992f,lambda*2.0f);
					
			/*if (lambda<=770.0f/2.0f && lambda>=390.0f/2.0f){	
				sprintf(buffer,"lambda: %f hue: %f",lambda,H);
				Ogre::LogManager::getSingletonPtr()->logMessage(buffer);
			}*/

			if (lambda>770.0f/2.0f) {
				H=0.0f;
				V=-1.0f/1730.0f*(lambda*2.0f)+250.0f/173.0f;
			}
			if (lambda<=390.0f/2.0f) {
						
				if (lambda>10.0f/2.0f) S=3.0f/1900.0f*(lambda*2.0f)+73.0f/190.0f;
				else {
					S=0.5f*(lambda*2.0f)-2.3f;
							
				}
			}

			if (H>320.0) H=320.0f;
			if (H<0.0) H=0.0f;
			if (V<0.0) V=0.0f;
			if (S<0.0) S=0.0f;
			if (S>1.0) S=1.0f;
			if (V>1.0) V=1.0f;
			
			int hi=(int)floor(H/60.0f);
					
			float f=H/60.0f-hi;
			float p=V*(1.0f-S);
			float q=V*(1.0f-S*f);
			float t=V*(1.0f-S*(1.0f-f));
			//sprintf(buffer,"c: %i H: %f",c, H);
		//Ogre::LogManager::getSingletonPtr()->logMessage(buffer);

			switch (hi)
			{
			case 0: red=V;green=t;blue=p;break;
			case 6: red=V;green=t;blue=p;break;
			case 1: red=q;green=V;blue=p;break;
			case 2: red=p;green=V;blue=t;break;
			case 3: red=p;green=q;blue=V;break;
			case 4: red=t;green=p;blue=V;break;
			case 5: red=V;green=p;blue=q;break;
					
			}

			rgb.push_back(red);
			rgb.push_back(green);
			rgb.push_back(blue);

			return rgb;

}

/**@name Vector operations
* These are some basic operations which are used to handle vectors of the type \ref vector3.
*/
//@{
/*! Builds the cross product of two given vectors
*/
	inline vector3 cross(vector3 *p1,vector3 *p2){
		vector3 c;
		c.x=p1->y*p2->z-p1->z*p2->y;
		c.y=p1->z*p2->x-p1->x*p2->z;
		c.z=p1->x*p2->y-p1->y*p2->x;
		return c;
	}

/*! Builds the dot product of two given vectors
*/
	inline float dot(vector3 *p1,vector3 *p2){
		return (p1->x*p2->x+p1->y*p2->y+p1->z*p2->z);
	}

/*! Calculates the length of a given vector
*/
	inline float length(vector3 *v){
		return sqrt(v->x*v->x+v->y*v->y+v->z*v->z);
	}

/*!Normalises a given vector
*/
	inline vector3 normalize(vector3 v){
		vector3 n;
		float length=sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
		if (length!=0){
		n.x=v.x/length;
		n.y=v.y/length;
		n.z=v.z/length;
		return n;}
		else {
			return v;
		}
	}
//@}


/*!Transforms a coulour of type Ogre::ColourValue into a vector of type \ref vector3. This simply copies the rgb-values into the vectors components with indices 0-2.*/
	inline vector3 colorToVector(Ogre::ColourValue colour){
		vector3 v;
		v.x=colour.r;
		v.y=colour.g;
		v.z=colour.b;
		return v;
	}


/*!This is only used when the lightning is calcuated on the CPU. 
@param amb the ambient lightning
@param diff the diffuse lightning
@param spec the specular lightning
@param shin the shininess of the material
@param L_in the colour of the object 
@param o_in the direction of the incoming ray of light
@param o_out the viewing vector
@param normal the normal of the object at this position
@param attC the constant attenuation
@param attL the linear attenuation
@param attQ the quadratic attenuation
@param distance the distance between the light source and the object
@returns A colour. The rgb values are stored in a vector's components.
*/
	inline vector3 calcBlinnPhongColor(vector3 amb,vector3 diff,vector3 spec,float shin,vector3 L_in,vector3 o_in,vector3 o_out,vector3 normal,float attC,float attL, float attQ,float distance){
//		char buffer[256];
		vector3 c;
		int v=1;
		float dot1=dot(&o_in,&normal);
		
		
		
		if (dot1<0) {
			dot1=0.0;
			v=0;
		} else{

		}
		vector3 half;
		half.x=o_in.x+o_out.x;
		half.y=o_in.y+o_out.y;
		half.z=o_in.z+o_out.z;
		half=normalize(half);
		float dot2=dot(&half,&normal);
		
		

		if (dot2<0){
			dot2=0.0;
		}else{

		}
	
		

		float att=attC+(attL+attQ*distance)*distance;
		c.x=L_in.x*(amb.x+(diff.x*dot1+v*spec.x*pow(dot2,shin))*att);
		c.y=L_in.y*(amb.y+(diff.y*dot1+v*spec.y*pow(dot2,shin))*att);
		c.z=L_in.z*(amb.z+(diff.z*dot1+v*spec.z*pow(dot2,shin))*att);
		/*if (c.x<0) {c.x=0;}
		if (c.y<0) {c.y=0;}
		if (c.z<0) {c.z=0;}*/
		if (c.x>1) {c.x=1.0;}
		if (c.y>1) {c.y=1.0;}
		if (c.z>1) {c.z=1.0;}

		/*if (c.z>1 || c.y>1 ||c.x>1){
			sprintf(buffer,"r: %f g: %f b: %f",c.x,c.y,c.z);
			Ogre::LogManager::getSingletonPtr()->logMessage(buffer);
		}*/

		return c;
		
	}
}
