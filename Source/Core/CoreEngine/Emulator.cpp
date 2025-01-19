#include "Emulator.h"

namespace NESEmu {
	static uint8_t NESFrameBuffer[NES_WIDTH * NES_HEIGHT * 3];

	uint8_t* GetFramebuffer() {
		return NESFrameBuffer;
	}

	inline void SetPixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
		if (x < 0 || x >= NES_WIDTH || y < 0 || y >= NES_HEIGHT) {
			throw "invalid px"; return;
		}
		NESFrameBuffer[(y * NES_WIDTH + x) * 3 + 0] = r;
		NESFrameBuffer[(y * NES_WIDTH + x) * 3 + 1] = g;
		NESFrameBuffer[(y * NES_WIDTH + x) * 3 + 2] = b;
	}

	void Clear() {
		memset(NESFrameBuffer, 0, NES_WIDTH * NES_HEIGHT * 3);
	}


	void Update(float Time, float DeltaTime)
	{
		Clear();

		for (int y = 0; y < NES_HEIGHT; ++y) {
			for (int x = 0; x < NES_WIDTH; ++x) {
				float X = (float)x / (float)NES_WIDTH;
				float Y = (float)y / (float)NES_HEIGHT;
				X = X * 2. - 1.;
				Y = Y * 2. - 1.;
				X *= 2.5f; Y *= 2.5f;

				if (X * X + Y * Y < (glm::sin(Time)*glm::sin(Time))) {
					NESFrameBuffer[(y * NES_WIDTH + x) * 3 + 0] = 255; // R
					NESFrameBuffer[(y * NES_WIDTH + x) * 3 + 1] = 255; // G
					NESFrameBuffer[(y * NES_WIDTH + x) * 3 + 2] = 255; // B
				}
				else {
					NESFrameBuffer[(y * NES_WIDTH + x) * 3 + 0] = 0; // R
					NESFrameBuffer[(y * NES_WIDTH + x) * 3 + 1] = 0; // G
					NESFrameBuffer[(y * NES_WIDTH + x) * 3 + 2] = 0; // B
				}
			}
		}
	}
}