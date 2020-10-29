"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports = (system) => {
    const buildSolution = system.getScript("/legacy/buildSolution.js");
    const solution = buildSolution(system);
    return {
        assignmentErrors: solution.assignmentErrors,
        assignments: solution.assignments,
        peripheralConfigurables: solution.peripheralConfigurables,
        peripheralConfigurations: solution.peripheralConfigurations,
        selectedUseCases: solution.selectedUseCases,
        version: system.version,
        deviceData: system.deviceData,
    };
};
