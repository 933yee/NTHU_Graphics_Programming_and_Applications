#version 330 core
out vec4 fragColor;
in vec2 TexCoords;
uniform sampler2D screenTexture;
uniform sampler2D noiseTexture;
uniform float time;   
uniform int mode;   
uniform vec2 mousePos;
uniform float splitPosition = 0.5;
uniform bool  isDragging = false;

vec4 quantize(vec4 color) {
    const float levels = 8.0;
    color.r = floor(color.r * levels) / levels;
    color.g = floor(color.g * levels) / levels;
    color.b = floor(color.b * levels) / levels;
    return color;
}

vec4 blur(sampler2D tex, vec2 coords, float radius) {
    vec4 colorSum = vec4(0.0);
    float weightSum = 0.0;
    const int samples = 9;
    
    for(int x = -samples; x <= samples; ++x) {
        for(int y = -samples; y <= samples; ++y) {
            vec2 offset = vec2(x, y) * radius;
            float dist = length(offset);
            float weight = exp(-dist * dist); 
            colorSum += texture(tex, coords + offset) * weight;
            weightSum += weight;
        }
    }
    
    return colorSum / weightSum;
}

vec4 applyEffect(vec2 coords) {
    if(mode == 1) {
        vec4 blurSmall = blur(screenTexture, coords, 0.0001); 
        vec4 blurLarge = blur(screenTexture, coords, 0.001);
        
        vec4 dogEdges = abs(blurSmall - blurLarge);
        float edgeIntensity = (dogEdges.r + dogEdges.g + dogEdges.b)/3;
        vec4 quantizedColor = quantize(blurSmall);
        const float edgeStrength = 5.0; 
        const float colorStrength = 1; 
        return mix(quantizedColor * colorStrength, vec4(0.0), edgeIntensity * edgeStrength);
    } 
    if(mode == 2) {
        vec4 blurredColor = blur(screenTexture, coords, 0.005);
        vec4 noiseColor = texture(noiseTexture, coords);
        vec2 distortedUV = coords + noiseColor.rg * 0.025;
        vec4 originalColor = texture(screenTexture, distortedUV);
        return quantize(originalColor);
    } 
    if(mode == 3) {
        float radius = 0.2;
        vec2 uvOffset = coords - mousePos; 
        float distance = length(uvOffset);
        if (distance < radius) {
            float magnificationFactor = 0.5f;
            float scale = 1.0 + (magnificationFactor - 1.0) * (1.0 - distance / radius);
            vec2 magnifiedCoords = mousePos + uvOffset * scale; 
            return texture(screenTexture, magnifiedCoords);
        }
    }
    if(mode == 4) {
        vec4 originalColor = texture(screenTexture, coords);
        const float bloomIntensity = 1.1; 
        vec4 bloomColor = blur(screenTexture, coords, 0.005);
        return originalColor*0.9 + (bloomColor * bloomIntensity);
    }
    if(mode == 5) {
        float pixelSize = 0.01f;
        vec2 pixelBlockCoords = floor(coords / pixelSize) * pixelSize;
        vec4 colorSum = vec4(0.0);
        int count = 0;
        
        for(int x = 0; x < 3; ++x) {
            for(int y = 0; y < 3; ++y) {
                vec2 offset = vec2(float(x) - 1.0, float(y) - 1.0) * pixelSize * 0.5;
                colorSum += texture(screenTexture, pixelBlockCoords + offset);
                count++;
            }
        }
        return colorSum / float(count);
    }
    if(mode == 6) {
        vec2 modifiedTexCoords = coords;
        modifiedTexCoords.x += 0.025f * sin(modifiedTexCoords.y * 2.0f * 3.14159265 + time);
        return texture(screenTexture, modifiedTexCoords);
    }
    return texture(screenTexture, coords);
}

void main() {
    float lineWidth = 0.002;
    float handleSize = 0.03; 
    float x = splitPosition;
    float distToSplit = abs(TexCoords.x - x);
    
    if (distToSplit < lineWidth && mode != 3 && mode != 0 && mode != 7) {
        fragColor = vec4(1.0, 1.0, 1.0, 0.8);
        return;
    }
    
    vec2 handleCenter = vec2(x, 0.5);
    vec2 diff = TexCoords - handleCenter;
    if (length(diff) < handleSize  && mode != 3 && mode != 0 && mode != 7) {
        float edgeDist = length(diff) / handleSize;
        if (edgeDist > 0.8) {
            fragColor = vec4(1.0, 1.0, 1.0, (1.0 - edgeDist) * 5.0);
            return;
        }
        fragColor = vec4(0.8, 0.8, 0.8, 0.6);
        return;
    }
    if (TexCoords.x < x  && mode != 3 ) {
        fragColor = texture(screenTexture, TexCoords);
    } else {
        fragColor = applyEffect(TexCoords);
    }
}