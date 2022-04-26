# Trabalho final

## Servidor Central

### MQTT Broker

Para o broker utilizado pelo o sistema é uma imagem docker do EMQX, configurada via docker-compose.

Com o serviço rodando é possivel acessar um dashboard do EMQX pela urls: `http://localhost:18083`. O sistema estará configurado com o usuário e senha padrão abaixo.


- usuário: **admin**
- senha: **public**

### Dashboard

O dashboard foi desenvolvido utilizando NextJS, seguindo o


### Como rodar

Ambos os serviços do dashboard e do MQTT broker estão dockerizados, de forma que para roda-los basta utilizar o comando abaixo:

```sh
docker-compose up
```

## ESP32
