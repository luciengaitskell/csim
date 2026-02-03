#include "chrono/core/ChDataPath.h"
#include "chrono/core/ChTimer.h"

#include "chrono_vehicle/ChVehicleDataPath.h"
#include "chrono_vehicle/terrain/RigidTerrain.h"
#include "chrono_vehicle/wheeled_vehicle/vehicle/WheeledVehicle.h"

#include <iostream>

using namespace chrono;
using namespace chrono::vehicle;

int main() {
  const std::string chrono_data_dir = CHRONO_DATA_DIR;
  SetChronoDataPath(chrono_data_dir);
  SetVehicleDataPath(chrono_data_dir + "vehicle/");

  const std::string veh_json =
      GetVehicleDataFile("hmmwv/vehicle/HMMWV_Vehicle.json");
  WheeledVehicle vehicle(veh_json, ChContactMethod::SMC,
                         /*create_powertrain=*/true, /*create_tires=*/true);
  vehicle.Initialize(ChCoordsys<>(ChVector3d(0, 0, 0.6), QUNIT));

  RigidTerrain terrain(vehicle.GetSystem());
  terrain.Initialize();

  double step = 1e-3;
  double t_end = 5.0;

  ChTimer timer;
  timer.start();

  for (;;) {
    auto sys = vehicle.GetSystem();
    double t = sys->GetChTime();
    if (t >= t_end)
      break;

    // Constant “driver inputs”
    DriverInputs inputs;
    inputs.m_throttle = 0.3;
    inputs.m_steering = 0.0;
    inputs.m_braking = 0.0;

    vehicle.Synchronize(t, inputs, terrain);
    terrain.Synchronize(t);

    sys->DoStepDynamics(step);
  }

  timer.stop();
  std::cout << "Simulated " << t_end << " s in " << timer() << " s wall time\n";
}
