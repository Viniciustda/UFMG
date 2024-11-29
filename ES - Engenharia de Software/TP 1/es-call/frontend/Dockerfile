FROM node:18.15.0

WORKDIR /usr/es-call-frontend

COPY package*.json ./

RUN npm install

COPY . .

EXPOSE 8080

CMD [ "npm", "start" ]