import AddCallForSupportTechnicianController from "./controller/AddCallForSupportTechnicianController";
import AddHighPriorityController from "./controller/call/AddHighPriorityController";
import AddLowPriorityController from "./controller/call/AddLowPriorityController";
import AddMediumPriorityController from "./controller/call/AddMediumPriorityController";
import CancelCallController from "./controller/call/CancelCallController";
import CreateNewCallController from "./controller/call/CreateNewCallController";
import FinishCallController from "./controller/call/FinishCallController";
import GetAllBySupportTechnicianController from "./controller/call/GetAllBySupportTechnicianController";
import GetAllCallsController from "./controller/GetAllCallsController";
import GetAllSupportTechnicianController from "./controller/GetAllSupportTechnicianController";
import Server, { HttpMethod } from "./infra/http/Server";

export default class Routes {

    static defineRoutes(server: Server) {
        server.on(HttpMethod.GET, "/call", new GetAllCallsController());
        server.on(HttpMethod.GET, "/call/support-technician/:supportTechnicianId", new GetAllBySupportTechnicianController())
        server.on(HttpMethod.PUT, "/call/add-support-technician", new AddCallForSupportTechnicianController());
        server.on(HttpMethod.PUT, "/call/set-low-priority", new AddLowPriorityController());
        server.on(HttpMethod.PUT, "/call/set-medium-priority", new AddMediumPriorityController());
        server.on(HttpMethod.PUT, "/call/set-high-priority", new AddHighPriorityController());
        server.on(HttpMethod.POST, "/call", new CreateNewCallController());
        server.on(HttpMethod.PUT, "/call/finish", new FinishCallController());
        server.on(HttpMethod.PUT, "/call/cancel", new CancelCallController());
        server.on(HttpMethod.GET, "/support-technician", new GetAllSupportTechnicianController());
    }
}