scene = document.getElementById("scene");
velText = document.getElementById("velocity_text");
stateText = document.getElementById("state_text");
oriText = document.getElementById("orientation_text");

let regex = /\[(.*?)\]/;

currentElement = undefined;

const url = "mqtt://127.0.0.1:9001"

const client = mqtt.connect(url);

client.on('connect', function () {})


AFRAME.registerComponent("clicable", {
  init: function () {
    let el = this.el;

    this.onRaycastHit = (evt) => {
      currentElement = el;
    };

    this.onRaycastClear = (evt) => {
      currentElement = undefined;
    };

    this.el.addEventListener("raycaster-intersected", this.onRaycastHit);
    this.el.addEventListener(
      "raycaster-intersected-cleared",
      this.onRaycastClear
    );
  },

  remove: function () {
    this.el.removeEventListener("raycaster-intersected", this.onRaycastHit);
    this.el.removeEventListener(
      "raycaster-intersected-cleared",
      this.onRaycastClear
    );
  },
});

AFRAME.registerComponent('led-commands',{
  init: function () {
    this.el.addEventListener('triggerdown', this.mqttCommandPublish);
  },
  mqttCommandPublish: function (evt) {
      console.log(currentElement)
      let value, match, parsedValue;
      if (currentElement != undefined){
        switch (currentElement.id) {
          case "increase_vel_button":
            value = velText.getAttribute('text')?.value;
            match = value && value.match(/\[(\d+)\]/);
            parsedValue = match ? parseInt(match[1], 10) : 0;
            if (parsedValue != 255) {
              parsedValue += 5;
              client.publish('conveyor/vel', `${parsedValue}`);
              changeText(velText, `Velocity[${parsedValue}]`);
            } else {
              console.log("parsedValue is 255");
            }
            break;
        
          case "decrease_vel_button":
            value = velText.getAttribute('text')?.value;
            match = value && value.match(/\[(\d+)\]/);
            parsedValue = match ? parseInt(match[1], 10) : 0;
            if (parsedValue != 0) {
              parsedValue -= 5;
              client.publish('conveyor/vel', `${parsedValue}`);
              changeText(velText, `Velocity[${parsedValue}]`);
            } else {
              console.log("parsedValue is 0");
            }
            break;

          case "state_button":
            value = stateText.getAttribute('text')?.value;
            match = value && value.match(/\[(.*?)\]/);
            
            if (match && match[1]) {
              let stateValue = match[1];
        
              if (stateValue === "Off") {
                client.publish('conveyor/state', '1');
                changeText(stateText, `State[On]`);
              } else if (stateValue === "On") {
                client.publish('conveyor/state', '0');
                changeText(stateText, `State[Off]`);
              } else {
                console.log("Valor inválido encontrado dentro dos colchetes:", stateValue);
              }
            } else {
              console.log("Padrão 'State[On]' ou 'State[Off]' não encontrado na string:", value);
            }
            break;

          case "orientation_button":
            value = oriText.getAttribute('text')?.value;
            match = value && value.match(/\[(.*?)\]/);
            
            if (match && match[1]) {
              let stateValue = match[1];
        
              if (stateValue === "Right") {
                client.publish('conveyor/orientation', '1');
                changeText(oriText, `Orientation[Left]`);
              } else if (stateValue === "Left") {
                client.publish('conveyor/orientation', '0');
                changeText(oriText, `Orientation[Right]`);
              } else {
                console.log("Valor inválido encontrado dentro dos colchetes:", stateValue);
              }
            } else {
              console.log("Padrão 'Orientation[Left]' ou 'Orientation[Right]' não encontrado na string:", value);
            }
            break;
        
          default:
            console.log("Invalid case");
            break;
        }
      }
  }
});