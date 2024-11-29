import HttpRestController from "../infra/http/HttpRestController";
import Request from "../infra/http/Request";
import Response from "../infra/http/Response";
import CallService from "../service/CallService";

export default class AddCallForSupportTechnicianController implements HttpRestController {

    private callService: CallService = new CallService();

    async execute(request: Request, response: Response): Promise<void> {
        try {
            const callId = request.getBody().callId;
            const supportTechnicianId = request.getBody().supportTechnicianId;

            await this.callService.addSupportTechnician(
                callId,
                supportTechnicianId
            );
            response.status(200).end()
        } catch (erro: any) {
            response.status(400).send({
                message: erro.message
            });
        }
    }
}