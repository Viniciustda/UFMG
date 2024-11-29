import ExpressServer from "./infra/http/express/ExpressServer"
import path from "path";
import Routes from "./Routes";

// Configurações servidor http
const directoryPathForPublicImages = path.join(__dirname, "..", "public");
const server = new ExpressServer(directoryPathForPublicImages);

Routes.defineRoutes(server);

server.listen(3000);

console.log("es-call backend running");
