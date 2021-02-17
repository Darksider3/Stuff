#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

vec4 MainColor() {
    return vec4(abs(sin(u_time/10.0)), abs(cos(u_time/10.0)), abs(atan(u_time/10.0)), clamp(sqrt(u_mouse.xy/u_resolution), 0.3, 0.9));
}

void main() {
    gl_FragColor = MainColor();
    return;
}
