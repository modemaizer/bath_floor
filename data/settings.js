function getSettings() {
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
        if (xhr.readyState == XMLHttpRequest.DONE) {
            var response = JSON.parse(xhr.responseText);
            document.getElementById("temperature").value = response.temperature;
            document.getElementById("delta").value = response.delta;
            document.getElementById("sensors_interval").value = response.sensors_interval;
            document.getElementById("mqtt_interval").value = response.mqtt_interval;
            document.getElementById("version").innerHTML = response.version;
        }
    }
    xhr.open("GET", "/settings/data", true);
    xhr.send();
  }
  
  function handleSubmit(event) {
    event.preventDefault();
    var myform = document.getElementById("myform");
    
    var formData = new FormData(myform);

    fetch("/settings/update", {
      method: "POST",
      body: formData,
    })
    .then(response => {
      if (!response.ok) {
        throw new Error('network returns error');
      }
      return response.json();
    })
    .then((resp) => {
      document.getElementById("temperature").value = resp.temperature;
      document.getElementById("delta").value = resp.delta;
      document.getElementById("sensors_interval").value = resp.sensors_interval;
      document.getElementById("mqtt_interval").value = resp.mqtt_interval;
      document.getElementById("version").innerHTML = resp.version;
      showMessage();
    })
    .catch((error) => {
      console.log("error ", error);
    });
  }

  const fadeIn = (el, timeout, display) => {
    el.style.opacity = 0;
    el.style.transition = `opacity ${timeout}ms`;
    setTimeout(() => {
      el.style.opacity = 1;
    }, 10);
  };

  const fadeOut = (el, timeout) => {
    el.style.opacity = 1;
    el.style.transition = `opacity ${timeout}ms`;
    
    setTimeout(() => {
      el.style.opacity = 0;
    }, timeout);
  };
  function showMessage() {
    var msg = document.getElementById("message");
    fadeIn(msg, 1000, 'flex');
    fadeOut(msg, 1000);
  }