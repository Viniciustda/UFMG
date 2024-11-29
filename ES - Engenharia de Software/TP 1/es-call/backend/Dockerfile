FROM node:18.15.0

WORKDIR /usr/es-call-backend

COPY package.json ./
COPY yarn.lock ./

RUN npm install

COPY . .
