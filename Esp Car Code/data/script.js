document.addEventListener('DOMContentLoaded', function() {
    const ws = new WebSocket('ws://' + window.location.hostname + ':81/');
    const mainContainer = document.getElementById('main-container');
    const menuButton = document.getElementById('menu');

    menuButton.addEventListener('click', function() {
        // Clear existing content
        mainContainer.innerHTML = '';

        // Create and add menu options to the main container
        const menuOptions = ['Print Stat', 'Program', 'Drive'];
        menuOptions.forEach(option => {
            const button = document.createElement('button');
            button.textContent = option;
            button.addEventListener('click', () => handleMenuSelection(option));
            mainContainer.appendChild(button);
        });
    });

    function handleMenuSelection(selection) {
        mainContainer.innerHTML = ''; // Clear the screen except for the menu button
        switch (selection) {
            case 'Print Stat':
                // Example: Create a component to display stats
                const statDisplay = document.createElement('div');
                statDisplay.textContent = 'Stats will be displayed here.';
                mainContainer.appendChild(statDisplay);
                // Additional logic to fetch and display stats from ESP
                createDataDisplay();
                break;
            case 'Program':
                // Example: Create input fields for distance and direction
                createInputForm();
                break;
            case 'Drive':
                // Logic to return to the drive control interface
                createDriveInterface();
                break;
        }
    }

    function createInputForm() {
        mainContainer.innerHTML = ''; // Clear the existing content

        // Create distance input field
        const distanceInput = document.createElement('input');
        distanceInput.type = 'number';
        distanceInput.placeholder = 'Distance (int)';
        distanceInput.id = 'distance';

        // Create direction input field
        const directionInput = document.createElement('input');
        directionInput.type = 'number';
        directionInput.placeholder = 'Direction (int)';
        directionInput.id = 'direction';

        // Create send button for distance
        const sendDistanceBtn = document.createElement('button');
        sendDistanceBtn.textContent = 'Send Distance';
        sendDistanceBtn.onclick = () => {
            const value = distanceInput.value;
            ws.send(`distValue:${value}`);
            console.log(`Distance sent: ${value}`); // Debugging
        };

        // Create send button for direction
        const sendDirectionBtn = document.createElement('button');
        sendDirectionBtn.textContent = 'Send Direction';
        sendDirectionBtn.onclick = () => {
            const value = directionInput.value;
            ws.send(`direValue:${value}`);
            console.log(`Direction sent: ${value}`); // Debugging
        };

        // Append elements to the main container
        mainContainer.appendChild(distanceInput);
        mainContainer.appendChild(sendDistanceBtn);
        mainContainer.appendChild(document.createElement('br')); // Line break for better layout
        mainContainer.appendChild(directionInput);
        mainContainer.appendChild(sendDirectionBtn);
    }


    function createDriveInterface() {
        mainContainer.innerHTML = ''; // Clear the existing content

        // Create horizontal slider for direction
        const horizontalSlider = document.createElement('input');
        horizontalSlider.type = 'range';
        horizontalSlider.min = '-100';
        horizontalSlider.max = '100';
        horizontalSlider.value = '0'; // Start in the middle
        horizontalSlider.classList.add('slider', 'horizontal-slider');

        // Create vertical slider for forward/backward
        const verticalSliderContainer = document.createElement('div');
        verticalSliderContainer.classList.add('vertical-slider-container');
        const verticalSlider = document.createElement('input');
        verticalSlider.type = 'range';
        verticalSlider.min = '-100';
        verticalSlider.max = '100';
        verticalSlider.value = '0'; // Start in the middle
        verticalSlider.classList.add('slider', 'vertical-slider');
        //  verticalSlider.style.writingMode = 'bt-lr'; // Makes the slider vertical

        // Function to handle slider value change
        const handleSliderChange = (event) => {
            const value = event.target.value;
            const direction = event.target === horizontalSlider ? 'H' : 'V';
            const message = `${direction}:${value}`;
            console.log(message);
            ws.send(message); // Send direction and value to ESP
        };

        // Function to reset slider value to 0
        const resetSlider = (event) => {
            event.target.value = '0';
            handleSliderChange(event); // Optionally send the reset value to ESP
        };

        // Attach event listeners for handling slider change
        horizontalSlider.addEventListener('input', handleSliderChange);
        verticalSlider.addEventListener('input', handleSliderChange);

        // Attach event listeners for resetting slider after release
        // For horizontal slider
        horizontalSlider.addEventListener('mouseup', resetSlider);
        horizontalSlider.addEventListener('touchend', resetSlider);
        // For vertical slider
        verticalSlider.addEventListener('mouseup', resetSlider);
        verticalSlider.addEventListener('touchend', resetSlider);

        // Append sliders to the main container
        mainContainer.appendChild(horizontalSlider);
        verticalSliderContainer.appendChild(verticalSlider);
        mainContainer.appendChild(verticalSliderContainer);
    }


    function createDataDisplay() {
        mainContainer.innerHTML = ''; // Clear the existing content

        // Create container for LIDAR data
        const lidarContainer = document.createElement('div');
        lidarContainer.id = 'lidarContainer';
        mainContainer.appendChild(lidarContainer);

        // Initialize the progress bar for LIDAR data
        let maxLidarValue = 100; // Assuming an initial max value; adjust as needed
        const lidarProgress = document.createElement('progress');
        const lidarValueText = document.createElement('span'); // Text for displaying the distance

        lidarProgress.id = 'lidarProgress';
        lidarProgress.value = maxLidarValue;
        lidarProgress.max = maxLidarValue;
        lidarContainer.appendChild(lidarProgress);
        lidarContainer.appendChild(lidarValueText); // Add the text element next to the progress bar

        // Function to update LIDAR data display
        window.updateLidarData = (newValue) => {
            lidarValueText.textContent = ` Distance: ${newValue}`; // Update text display
            lidarProgress.value = maxLidarValue - newValue; // Invert progress bar filling
            if (newValue > maxLidarValue) {
                maxLidarValue = newValue; // Update max value if the new value is larger
                lidarProgress.max = maxLidarValue;
                lidarProgress.value = maxLidarValue - newValue; // Recalculate current value
            }
        };


          // Create compass display container
          const compassContainer = document.createElement('div');
          compassContainer.id = 'compassContainer';
          compassContainer.style.position = 'relative';
          compassContainer.style.width = '200px';
          compassContainer.style.height = '200px';
          compassContainer.style.borderRadius = '50%';
          compassContainer.style.border = '2px solid #000';
          compassContainer.style.display = 'flex';
          compassContainer.style.justifyContent = 'center';
          compassContainer.style.alignItems = 'center';
          compassContainer.style.margin = '20px auto';
          mainContainer.appendChild(compassContainer);

          // Create the compass directions (N, S, E, W)
          ['N', 'S', 'E', 'W'].forEach(direction => {
              const dirElement = document.createElement('div');
              dirElement.textContent = direction;
              dirElement.style.position = 'absolute';
              dirElement.style.fontWeight = 'bold';
              switch (direction) {
                  case 'N': dirElement.style.top = '0'; break;
                  case 'S': dirElement.style.bottom = '0'; break;
                  case 'E': dirElement.style.right = '0'; break;
                  case 'W': dirElement.style.left = '0'; break;
              }
              compassContainer.appendChild(dirElement);
          });

          // Create the indicator for the current direction
          const indicator = document.createElement('div');
          indicator.id = 'indicator';
          indicator.style.position = 'absolute';
          indicator.style.width = '20px'; // Equal width and height to form a circle
          indicator.style.height = '20px';
          indicator.style.backgroundColor = 'red';
          indicator.style.borderRadius = '50%'; // Makes the div round
          indicator.style.transform = 'translate(-50%, -50%)';
          indicator.style.left = '50%'; // Align center of indicator with center of compass container
          indicator.style.top = '50%';
          compassContainer.appendChild(indicator);

        // Adjusting the indicator to move along the perimeter
        const compassValueText = document.createElement('div'); // Text for displaying the direction
        compassValueText.style.position = 'absolute';
        compassValueText.style.transform = 'translate(-50%, -50%)';
        compassValueText.id = 'compassValueText';
        compassContainer.appendChild(compassValueText); // Add the text element to the center

        // Function to update compass direction
        window.updateCompassDirection = (degrees) => {
            // Calculate the indicator's position on the circle perimeter
            const radius = 100; // Adjust based on the size of your compass container
            const radians = degrees * (Math.PI / 180);
            // Adjust for the center of the indicator
            const indicatorRadius = 10; // Half the size of the indicator for centering
            const x = (radius - indicatorRadius) * Math.cos(radians);
            const y = (radius - indicatorRadius) * Math.sin(radians);

            // Update the position of the indicator
            indicator.style.left = `calc(50% + ${x}px)`;
            indicator.style.top = `calc(50% + ${y}px)`;

            // Update the text with the degree value
            compassValueText.textContent = `${degrees}°`;
            compassValueText.style.left = '50%';
            compassValueText.style.top = '50%';
        };
    }

    // WebSocket event listeners
    ws.onopen = function() {
        console.log("WebSocket connection established");
    };

    ws.onmessage = function(event) {
        const data = event.data;
        const parts = data.split(',');
        if (parts.length === 2) {
            const distance = parseInt(parts[0], 10);
            const direction = parseInt(parts[1], 10);

            //console.log("Distance:", distance, "Direction:", direction);
            updateLidarData(distance);
        updateCompassDirection(direction);
        }
    };

    // Initial call to create the drive interface or another default state
    createDriveInterface();
});
