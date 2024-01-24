# ImanolAitor_EXT_EntregaFinal

Este proyecto ha sido completado por Imanol Anda y Aitor Salazar. La programación del EK-RA4M1 se encuentra en los archivos .c y .h, así como en el archivo comprimido. La programación relacionada con la Raspberry Pi se encuentra en el archivo raspberry.py. Además, la programación del cliente MQTT se halla en el archivo refresh.py. Ademas, del diagrama de flujo sobre la implentación del proyecto. La programación del proyecto entero se encuentra en el programa.zip.

## Desarrollo
Para activar nuestro sistema de seguridad, se debe presionar un pulsador. Al hacerlo, el display LCD mostrará 'SYSTEM ON' y el sistema permanecerá activo mientras la EK esté encendida. El sistema leerá constantemente la velocidad del vehículo, que simulamos a través de un potenciómetro conectado al módulo ADC. Utilizamos un filtro de mediana móvil para filtrar valores anómalos.

Todos los datos filtrados se envían a la Raspberry Pi mediante UART, donde se guardan en un log con un timestamp. Si la velocidad supera el 75% de la velocidad máxima, considerada como umbral de seguridad, la EK emitirá un comando de freno de emergencia. Esto implica parpadear un LED y mostrar en el LCD la velocidad actual y la palabra 'PELIGRO'. Además, la EK hará sonar un zumbador durante un segundo.

En este momento, la Raspberry Pi publicará en un broker MQTT un mensaje con la velocidad y un timestamp. Este mensaje se enviará solo una vez cada vez que se active el freno de emergencia. Cuando la velocidad disminuya y esté por debajo del umbral, el LED dejará de parpadear y el LCD mostrará nuevamente 'SYSTEM ON'.

Finalmente, desde un ordenador portátil, se estará ejecutando un cliente MQTT que escuchará continuamente las publicaciones en este topic. Cuando se reciba un mensaje de freno de emergencia, se mostrará en la terminal del ordenador


## Funcionamiento
Primero, debemos realizar las conexiones correctamente tanto en la EK-RA4M1 como en la Raspberry Pi. Después, abrimos el proyecto completo en el E2 Studio y lo ejecutamos. Seguidamente, abrimos dos instancias de Visual Studio Code y accedemos a nuestra Raspberry Pi, en mi caso, mediante SSH utilizando Imanolanda@imanolpi. Una vez dentro, navegamos hasta el directorio donde se encuentran nuestros archivos .py. En mi caso, debo ejecutar un 'cd desktop' seguido de un 'cd Raspberry'. Ya en ese directorio, ejecutamos ambos archivos .py, comenzando con refresh.py y luego con raspberry.py. Esto nos generará un archivo .csv en la misma ubicación de los dos archivos .py.
