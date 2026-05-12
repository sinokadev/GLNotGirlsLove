#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform float u_time;
uniform float u_blurStrength = 0.01;

vec2 hash(vec2 p) {
    p = vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)));
    return -1.0 + 2.0 * fract(sin(p) * 43758.5453123);
}

float perlin_noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);

    vec2 u = f * f * f * (f * (f * 6.0 - 15.0) + 10.0);

    return mix(mix(dot(hash(i + vec2(0.0, 0.0)), f - vec2(0.0, 0.0)),
                   dot(hash(i + vec2(1.0, 0.0)), f - vec2(1.0, 0.0)), u.x),
               mix(dot(hash(i + vec2(0.0, 1.0)), f - vec2(0.0, 1.0)),
                   dot(hash(i + vec2(1.0, 1.0)), f - vec2(1.0, 1.0)), u.x), u.y);
}

void main() {
    vec2 uv = TexCoords;

    float n = perlin_noise(uv * 15.0);

    float color = n * 0.5 + 0.5;

    float binary = smoothstep(0.5 - u_blurStrength, 0.5 + u_blurStrength, color);

    vec3 color1 = vec3(255.0/255.0, 242.0/255.0, 207.0/255.0);
    vec3 color2 = vec3(255.0/255.0, 192.0/255.0, 23.0/255.0);

    vec3 result = mix(color1, color2, binary);
    
    FragColor = vec4(result, 1.0);
}