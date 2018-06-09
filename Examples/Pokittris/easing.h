// t = time, b = start, c = end, d = duration
//float PI = 3.141591;

float easeInQuad(float t, float b, float c, float d) {
	return c*(t/=d)*t + b;
}
float easeOutQuad(float t, float b, float c, float d) {
	return -c *(t/=d)*(t-2) + b;
}
float easeInOutQuad(float t, float b, float c, float d) {
	if ((t/=d/2) < 1) return c/2*t*t + b;
	return -c/2 * ((--t)*(t-2) - 1) + b;
}
float easeInCubic(float t, float b, float c, float d) {
	return c*(t/=d)*t*t + b;
}
float easeOutCubic(float t, float b, float c, float d) {
	return c*((t=t/d-1)*t*t + 1) + b;
}
float easeInOutCubic(float t, float b, float c, float d) {
	if ((t/=d/2) < 1) return c/2*t*t*t + b;
	return c/2*((t-=2)*t*t + 2) + b;
}
float easeInQuart(float t, float b, float c, float d) {
	return c*(t/=d)*t*t*t + b;
}
float easeOutQuart(float t, float b, float c, float d) {
	return -c * ((t=t/d-1)*t*t*t - 1) + b;
}
float easeInOutQuart(float t, float b, float c, float d) {
	if ((t/=d/2) < 1) return c/2*t*t*t*t + b;
	return -c/2 * ((t-=2)*t*t*t - 2) + b;
}
float easeInQuint(float t, float b, float c, float d) {
	return c*(t/=d)*t*t*t*t + b;
}
float easeOutQuint(float t, float b, float c, float d) {
	return c*((t=t/d-1)*t*t*t*t + 1) + b;
}
float easeInOutQuint(float t, float b, float c, float d) {
	if ((t/=d/2) < 1) return c/2*t*t*t*t*t + b;
	return c/2*((t-=2)*t*t*t*t + 2) + b;
}
float easeInSine(float t, float b, float c, float d) {
	return -c * cos(t/d * (PI/2)) + c + b;
}
float easeOutSine(float t, float b, float c, float d) {
	return c * sin(t/d * (PI/2)) + b;
}
float easeInOutSine(float t, float b, float c, float d) {
	return -c/2 * (cos(PI*t/d) - 1) + b;
}
float easeInExpo(float t, float b, float c, float d) {
	return (t==0) ? b : c * pow(2, 10 * (t/d - 1)) + b;
}
float easeOutExpo(float t, float b, float c, float d) {
	return (t==d) ? b+c : c * (-pow(2, -10 * t/d) + 1) + b;
}
float easeInOutExpo(float t, float b, float c, float d) {
	if (t==0) return b;
	if (t==d) return b+c;
	if ((t/=d/2) < 1) return c/2 * pow(2, 10 * (t - 1)) + b;
	return c/2 * (-pow(2, -10 * --t) + 2) + b;
}
float easeInCirc(float t, float b, float c, float d) {
	return -c * (sqrt(1 - (t/=d)*t) - 1) + b;
}
float easeOutCirc(float t, float b, float c, float d) {
	return c * sqrt(1 - (t=t/d-1)*t) + b;
}
float easeInOutCirc(float t, float b, float c, float d) {
	if ((t/=d/2) < 1) return -c/2 * (sqrt(1 - t*t) - 1) + b;
	return c/2 * (sqrt(1 - (t-=2)*t) + 1) + b;
}
float easeInElastic(float t, float b, float c, float d) {
	float s=1.70158;float p=0; float a=c;
	if (t==0) return b;  if ((t/=d)==1) return b+c;  if (!p) p=d*.3;
	if (a < abs((int)c)) { a=c; s=p/4; }
	else  s = p/(2*PI) * asin (c/a);
	return -(a*pow(2,10*(t-=1)) * sin( (t*d-s)*(2*PI)/p )) + b;
}
float easeOutElastic(float t, float b, float c, float d) {
	float s=1.70158;float p=0;float a=c;
	if (t==0) return b;  if ((t/=d)==1) return b+c;  if (!p) p=d*.3;
	if (a < abs((int)c)) { a=c; s=p/4; }
	else s = p/(2*PI) * asin (c/a);
	return a*pow(2,-10*t) * sin( (t*d-s)*(2*PI)/p ) + c + b;
}
float easeInOutElastic(float t, float b, float c, float d) {
	float s=1.70158;float p=0;float a=c;
	if (t==0) return b;  if ((t/=d/2)==2) return b+c;  if (!p) p=d*(.3*1.5);
	if (a < abs((int)c)) { a=c; s=p/4; }
	else s = p/(2*PI) * asin (c/a);
	if (t < 1) return -.5*(a*pow(2,10*(t-=1)) * sin( (t*d-s)*(2*PI)/p )) + b;
	return a*pow(2,-10*(t-=1)) * sin( (t*d-s)*(2*PI)/p )*.5 + c + b;
}
float easeInBack(float t, float b, float c, float d) {
	float s = 1.70158;
	return c*(t/=d)*t*((s+1)*t - s) + b;
}
float easeOutBack(float t, float b, float c, float d) {
	float s = 1.70158;
	return c*((t=t/d-1)*t*((s+1)*t + s) + 1) + b;
}
float easeInOutBack(float t, float b, float c, float d) {
	float s = 1.70158;
	if ((t/=d/2) < 1) return c/2*(t*t*(((s*=(1.525))+1)*t - s)) + b;
	return c/2*((t-=2)*t*(((s*=(1.525))+1)*t + s) + 2) + b;
}

float easeOutBounce(float t, float b, float c, float d) {
	if ((t/=d) < (1/2.75)) {
		return c*(7.5625*t*t) + b;
	} else if (t < (2/2.75)) {
		return c*(7.5625*(t-=(1.5/2.75))*t + .75) + b;
	} else if (t < (2.5/2.75)) {
		return c*(7.5625*(t-=(2.25/2.75))*t + .9375) + b;
	} else {
		return c*(7.5625*(t-=(2.625/2.75))*t + .984375) + b;
	}
}
float easeInBounce(float t, float b, float c, float d) {
	return c - easeOutBounce (d-t, 0, c, d) + b;
}
float easeInOutBounce(float t, float b, float c, float d) {
	if (t < d/2) return easeInBounce (t*2, 0, c, d) * .5 + b;
	return easeOutBounce (t*2-d, 0, c, d) * .5 + c*.5 + b;
}
