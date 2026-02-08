#include "chrono/core/ChDataPath.h"
#include "chrono/core/ChTimer.h"

#include "chrono_vehicle/ChPowertrainAssembly.h"
#include "chrono_vehicle/ChSubsysDefs.h"
#include "chrono_vehicle/ChVehicleDataPath.h"
#include "chrono_vehicle/terrain/FlatTerrain.h"
#include "chrono_vehicle/utils/ChUtilsJSON.h"
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
  WheeledVehicle vehicle(veh_json, ChContactMethod::NSC,
                         /*create_powertrain=*/true, /*create_tires=*/true);
  vehicle.Initialize(ChCoordsys<>(ChVector3d(0, 0, 0.6), QUNIT));

  // Attach tires (the HMMWV vehicle JSON does not specify tire files)
  const std::string tire_json =
      GetVehicleDataFile("hmmwv/tire/HMMWV_TMsimpleTire.json");
  for (unsigned int axle = 0; axle < vehicle.GetNumberAxles(); ++axle) {
    auto tire_l = ReadTireJSON(tire_json);
    auto tire_r = ReadTireJSON(tire_json);
    vehicle.InitializeTire(tire_l, vehicle.GetWheel(axle, VehicleSide::LEFT),
                           VisualizationType::PRIMITIVES,
                           ChTire::CollisionType::SINGLE_POINT);
    vehicle.InitializeTire(tire_r, vehicle.GetWheel(axle, VehicleSide::RIGHT),
                           VisualizationType::PRIMITIVES,
                           ChTire::CollisionType::SINGLE_POINT);
  }

  // Attach powertrain (engine + transmission)
  const std::string engine_json =
      GetVehicleDataFile("hmmwv/powertrain/HMMWV_EngineSimpleMap.json");
  const std::string transmission_json = GetVehicleDataFile(
      "hmmwv/powertrain/HMMWV_AutomaticTransmissionSimpleMap.json");
  auto engine = ReadEngineJSON(engine_json);
  auto transmission = ReadTransmissionJSON(transmission_json);
  auto powertrain =
      chrono_types::make_shared<ChPowertrainAssembly>(engine, transmission);
  vehicle.InitializePowertrain(powertrain);

  // Simple infinite flat terrain (suitable for TMsimple tires)
  FlatTerrain terrain(0.0, 0.9f);

  double step = 1e-3;
  double t_end = 10.0;

  auto print_state = [&](const char *label) {
    const auto pos = vehicle.GetPos();
    const double speed = vehicle.GetSpeed();
    std::cout << label << " position: (" << pos.x() << ", " << pos.y() << ", "
              << pos.z() << "), speed: " << speed << " m/s\n";
  };

  print_state("Start");

  ChTimer timer;
  timer.start();

  auto sys = vehicle.GetSystem();
  // can enable threading, but pointless for this simple example
  // sys->SetNumThreads(2);
  for (;;) {
    double t = sys->GetChTime();
    if (t >= t_end)
      break;

    // Constant “driver inputs”
    DriverInputs inputs;
    inputs.m_throttle = 0.2;
    inputs.m_steering = 0.0;
    inputs.m_braking = 0.0;

    vehicle.Synchronize(t, inputs, terrain);
    terrain.Synchronize(t);

    terrain.Advance(step);
    vehicle.Advance(step);
  }

  timer.stop();
  print_state("End");
  std::cout << "Simulated " << t_end << " s in " << timer() << " s wall time\n";
}
