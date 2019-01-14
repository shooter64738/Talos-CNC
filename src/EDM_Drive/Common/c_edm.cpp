/*
*  c_edm.cpp - NGC_RS274 controller.
*  A component of Talos
*
*  Copyright (c) 2016-2019 Jeff Dill
*
*  Talos is free software: you can redistribute it and/or modify
*  it under the terms of the GNU LPLPv3 License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  Talos is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with Talos.  If not, see <http://www.gnu.org/licenses/>.
*/


/*
1. We need to know what the gap voltage should be
2. We need to determine a feed rate appropriate to maintain that gap voltage
(The gap voltage is the voltage that the EDM should maintain during a cutting operation.
If the voltage drops to far, then we must slow down, if the voltage rises we msut speed up.)
3. If at some point X/Y axis are added for a full EDM system, they will also have to fall 
within these speed parameters. Everything in motion is tied to the gap voltage.
*/

uint16_t c_edm::Set_ArcVoltage(float voltage)
{

}
