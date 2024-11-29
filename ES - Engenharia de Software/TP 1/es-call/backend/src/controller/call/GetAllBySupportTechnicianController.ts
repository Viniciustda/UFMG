import HttpRestController from "../../infra/http/HttpRestController";
import Request from "../../infra/http/Request";
import Response from "../../infra/http/Response";
import CallService from "../../service/CallService";

export default class GetAllBySupportTechnicianController implements HttpRestController {

    private callService: CallService = new CallService();

    async execute(request: Request, response: Response): Promise<void> {
        try {
            const supportTechnicianId = request.getParams().supportTechnicianId;
            const calls = await this.callService.getAllBySupportTechnician(supportTechnicianId);
            response.status(200).send(calls);
        } catch (erro: any) {
            response.status(400).send({
                message: erro.message
            });
        }
    }
}