// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE.txt for full details.
// ----------------------------------------------------------------

// Request GLSL 3.3
//#version 330

// This corresponds to the output color to the color buffer
//out vec4 outColor;

//void main()
//{
	// Sample color from texture
//    outColor = vec4(1.0, 1.0, 1.0, 1.0); // Cor branca
//}

#version 330 core

// Uniform para a cor do objeto
uniform vec3 uColor;

// Output para o frame buffer
out vec4 fragColor;

void main()
{
    // Define a cor do fragmento a partir da uniform, com alfa em 1.0
    fragColor = vec4(uColor, 1.0);
}