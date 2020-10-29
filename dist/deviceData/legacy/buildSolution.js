"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
function getConfigurableType(configurable) {
    return configurable.type.toLowerCase();
}
function isLegacy(configurable) {
    return configurable.legacy;
}
function isCheckBox(configurable) {
    return getConfigurableType(configurable) === "checkbox";
}
function isDropDown(configurable) {
    return getConfigurableType(configurable) === "dropdown";
}
function createEmptySolution() {
    return {
        assignments: [],
        assignmentErrors: [],
        peripheralConfigurables: [],
        peripheralConfigurations: {},
        selectedUseCases: {},
    };
}
exports.createEmptySolution = createEmptySolution;
const overrideMemberName = "$assign";
function isGPIOInterfaceInstance(inst) {
    return !(overrideMemberName in inst);
}
function isInterfaceInstance(inst) {
    return overrideMemberName in inst;
}
function createConfigValuesObj(configurables, inst) {
    const obj = {};
    _.each(configurables, (c) => {
        obj[c.name] = isDropDown(c) ?
            _.find(c.options, (o) => o.name === inst[c.name]) :
            inst[c.name];
    });
    return obj;
}
const buildSolution = (system) => {
    const solution = createEmptySolution();
    const deviceData = system.deviceData;
    const interfaces = system.peripherals;
    const powerDomainInfo = {
        enabled: system.powerDomains.areEnabled(),
        settings: system.powerDomains.getSettings(),
    };
    _.each(interfaces, (iface) => {
        _.each(iface.$instances, (inst) => {
            if (isInterfaceInstance(inst)) {
                const useCaseOption = inst.$useCase;
                const selectedUseCase = inst.$interface.useCases[useCaseOption];
                solution.selectedUseCases[inst.$name] = selectedUseCase;
                const configurables = inst.$interface.configurables;
                const peripheralName = inst.$solution.peripheralName;
                const voltage = inst.$solution.voltage;
                const ioSet = inst.$solution.ioSet;
                if (configurables && configurables.length > 0 && inst.$solution.peripheralName !== "") {
                    const configurableValuesObj = createConfigValuesObj(configurables, inst);
                    solution.peripheralConfigurations[peripheralName] = configurableValuesObj;
                    solution.peripheralConfigurables.push({
                        configurables: configurableValuesObj,
                        interfaceName: inst.$interface.name,
                        peripheral: peripheralName,
                        requirementName: inst.$name,
                    });
                }
                const allPins = _.compact([
                    ..._.map(selectedUseCase.useCasePins, (ucPin) => inst[ucPin.name]),
                    ..._.flatMap(selectedUseCase.useCasePinSets, (pinSet) => _.map(pinSet.useCasePins, (ucPin) => inst.$pinGroup[ucPin.name])),
                ]);
                _.each(allPins, (pinInst) => {
                    createAssignmentObj(pinInst, inst.$interface.name, inst.$name, peripheralName, voltage, ioSet, selectedUseCase);
                });
            }
            else if (isGPIOInterfaceInstance(inst)) {
                console.assert(_.size(inst.$interface.useCases) === 1, "GPIO interfaces should not have anything other than default use cases");
                const selectedUseCase = _.values(inst.$interface.useCases)[0];
                solution.selectedUseCases[inst.$name] = selectedUseCase;
                for (let i = 0; i < Number(inst.$numPins); i++) {
                    const pinInstances = inst;
                    const pinInst = pinInstances[i.toString()];
                    const perfPinName = pinInst.$solution.peripheralPinName;
                    const peripheralName = (perfPinName ?
                        _.find(inst.$interface.peripherals, (peripheral) => !!peripheral.peripheralPins[perfPinName]).name :
                        "");
                    createAssignmentObj(pinInst, inst.$interface.name, inst.$name, peripheralName, null, null, selectedUseCase);
                }
            }
        });
    });
    function addLegacyConfigurables(pinInstance, assignment) {
        const pinType = pinInstance.$interfacePins[0];
        _.each(pinType.configurables, (configurable) => {
            if (isLegacy(configurable)) {
                if (isDropDown(configurable)) {
                    const selectedOption = _.find(configurable.options, (o) => o.name === pinInstance[configurable.name]);
                    _.each(configurable.options, (option) => {
                        assignment[option.name] = option.name === selectedOption.name;
                    });
                }
                else if (isCheckBox(configurable)) {
                    assignment[configurable.name] = pinInstance[configurable.name];
                }
            }
        });
    }
    function createAssignmentObj(pinInstance, interfaceName, parentReqName, peripheralName, voltage, ioSet, useCase) {
        const warning = pinInstance.$solution.warning;
        const configurables = createConfigValuesObj(pinInstance.$interfacePins[0].configurables, pinInstance);
        if (pinInstance.$solution.devicePinName) {
            const assignment = {
                configurables,
                devicePin: deviceData.devicePins[pinInstance.$solution.packagePinName],
                interfaceName,
                peripheral: deviceData.peripherals[peripheralName],
                peripheralPin: deviceData.peripheralPins[pinInstance.$solution.peripheralPinName],
                requirementName: parentReqName,
                useCase: useCase.description,
                warning,
            };
            if (powerDomainInfo.enabled) {
                assignment.powerSetting = powerDomainInfo.settings[assignment.devicePin.powerDomain.name];
                assignment.requiredVoltageLevel = voltage;
            }
            if (ioSet) {
                assignment.ioSet = ioSet;
            }
            assignment.muxMode = _.find(assignment.devicePin.mux.muxSetting, (muxSetting) => muxSetting.peripheralPin === assignment.peripheralPin).mode;
            addLegacyConfigurables(pinInstance, assignment);
            solution.assignments.push(assignment);
        }
        else if (pinInstance.$solution.error !== "") {
            const assignment = {
                configurables,
                error: pinInstance.$solution.error,
                warning,
            };
            addLegacyConfigurables(pinInstance, assignment);
            solution.assignmentErrors.push(assignment);
        }
    }
    return solution;
};
exports = buildSolution;
