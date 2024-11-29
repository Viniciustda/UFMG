import HttpRestController from "../infra/http/HttpRestController";
import Request from "../infra/http/Request";
import Response from "../infra/http/Response";
import SupportTechnicianService from "../service/SupportTechnicianService";

export default class GetAllSupportTechnicianController implements HttpRestController {

    private supportTechnicianService: SupportTechnicianService = new SupportTechnicianService(); 

    async execute(request: Request, response: Response): Promise<void> {
        const supportTechnicians = await this.supportTechnicianService.getAll();
        response.status(200).send(supportTechnicians);
    }
}