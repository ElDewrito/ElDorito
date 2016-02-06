#include "Assassination.hpp"
#include "../ElDorito.hpp"
#include "../Blam/Tags/ModelAnimationGraph.hpp"

namespace
{
	bool assassinationEnabled = true;
	Blam::Tags::ModelAnimationGraph* spartan = Blam::Tags::GetTag<Blam::Tags::ModelAnimationGraph>(0x0D76);
}

namespace Patches
{
	namespace Assassination
	{
		void Enable(bool enabled)
		{
			assassinationEnabled = enabled;
			if (enabled) {
				spartan->Animations[0].FrameCount = 30;
				spartan->Animations[1].FrameCount = 75;
				spartan->Animations[2].FrameCount = 77;
				spartan->Animations[3].FrameCount = 75;
				spartan->Animations[4].FrameCount = 76;
				spartan->Animations[5].FrameCount = 75;
				spartan->Animations[6].FrameCount = 75;
				spartan->Animations[7].FrameCount = 78;
				spartan->Animations[8].FrameCount = 30;
				spartan->Animations[9].FrameCount = 76;
				spartan->Animations[10].FrameCount = 75;
				spartan->Animations[11].FrameCount = 75;
				spartan->Animations[12].FrameCount = 75;
				spartan->Animations[13].FrameCount = 75;
				spartan->Animations[14].FrameCount = 75;
				spartan->Animations[15].FrameCount = 75;
				spartan->Animations[16].FrameCount = 30;
				spartan->Animations[17].FrameCount = 30;
				spartan->Animations[18].FrameCount = 75;
				spartan->Animations[19].FrameCount = 75;
				spartan->Animations[20].FrameCount = 76;
				spartan->Animations[21].FrameCount = 76;
				spartan->Animations[22].FrameCount = 79;
				spartan->Animations[23].FrameCount = 79;
				spartan->Animations[24].FrameCount = 75;
				spartan->Animations[25].FrameCount = 75;
				spartan->Animations[26].FrameCount = 75;
				spartan->Animations[27].FrameCount = 90;
				spartan->Animations[28].FrameCount = 75;
				spartan->Animations[29].FrameCount = 75;
				spartan->Animations[30].FrameCount = 75;
				spartan->Animations[31].FrameCount = 75;
				spartan->Animations[32].FrameCount = 76;
				spartan->Animations[33].FrameCount = 76;
				spartan->Animations[34].FrameCount = 79;
				spartan->Animations[35].FrameCount = 79;
				spartan->Animations[36].FrameCount = 104;
				spartan->Animations[37].FrameCount = 79;
				spartan->Animations[38].FrameCount = 30;
				spartan->Animations[39].FrameCount = 75;
				spartan->Animations[40].FrameCount = 75;
				spartan->Animations[41].FrameCount = 30;
				spartan->Animations[42].FrameCount = 75;
				spartan->Animations[43].FrameCount = 75;
				spartan->Animations[44].FrameCount = 30;
				spartan->Animations[45].FrameCount = 30;
				spartan->Animations[46].FrameCount = 75;
				spartan->Animations[47].FrameCount = 75;
				spartan->Animations[48].FrameCount = 75;
				spartan->Animations[49].FrameCount = 75;
			}
			else {
				spartan->Animations[0].FrameCount = 0;
				spartan->Animations[1].FrameCount = 0;
				spartan->Animations[2].FrameCount = 0;
				spartan->Animations[3].FrameCount = 0;
				spartan->Animations[4].FrameCount = 0;
				spartan->Animations[5].FrameCount = 0;
				spartan->Animations[6].FrameCount = 0;
				spartan->Animations[7].FrameCount = 0;
				spartan->Animations[8].FrameCount = 0;
				spartan->Animations[9].FrameCount = 0;
				spartan->Animations[10].FrameCount = 0;
				spartan->Animations[11].FrameCount = 0;
				spartan->Animations[12].FrameCount = 0;
				spartan->Animations[13].FrameCount = 0;
				spartan->Animations[14].FrameCount = 0;
				spartan->Animations[15].FrameCount = 0;
				spartan->Animations[16].FrameCount = 0;
				spartan->Animations[17].FrameCount = 0;
				spartan->Animations[18].FrameCount = 0;
				spartan->Animations[19].FrameCount = 0;
				spartan->Animations[20].FrameCount = 0;
				spartan->Animations[21].FrameCount = 0;
				spartan->Animations[22].FrameCount = 0;
				spartan->Animations[23].FrameCount = 0;
				spartan->Animations[24].FrameCount = 0;
				spartan->Animations[25].FrameCount = 0;
				spartan->Animations[26].FrameCount = 0;
				spartan->Animations[27].FrameCount = 0;
				spartan->Animations[28].FrameCount = 0;
				spartan->Animations[29].FrameCount = 0;
				spartan->Animations[30].FrameCount = 0;
				spartan->Animations[31].FrameCount = 0;
				spartan->Animations[32].FrameCount = 0;
				spartan->Animations[33].FrameCount = 0;
				spartan->Animations[34].FrameCount = 0;
				spartan->Animations[35].FrameCount = 0;
				spartan->Animations[36].FrameCount = 0;
				spartan->Animations[37].FrameCount = 0;
				spartan->Animations[38].FrameCount = 0;
				spartan->Animations[39].FrameCount = 0;
				spartan->Animations[40].FrameCount = 0;
				spartan->Animations[41].FrameCount = 0;
				spartan->Animations[42].FrameCount = 0;
				spartan->Animations[43].FrameCount = 0;
				spartan->Animations[44].FrameCount = 0;
				spartan->Animations[45].FrameCount = 0;
				spartan->Animations[46].FrameCount = 0;
				spartan->Animations[47].FrameCount = 0;
				spartan->Animations[48].FrameCount = 0;
				spartan->Animations[49].FrameCount = 0;
			}
		}
	}
}