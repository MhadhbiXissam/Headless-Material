#version 300 es
precision highp float;
in vec2 vTex;
out vec4 FragColor;
uniform float uTime;
uniform vec4 uColor;
uniform sampler2D uTex;
void main() {
  vec3 texCol = texture(uTex, vTex).rgb;
  vec3 col = 0.5 + 0.5*cos(uTime + vTex.xyx + vec3(0,2,4));
  FragColor = vec4(col * texCol * uColor.rgb,1.0);
}
