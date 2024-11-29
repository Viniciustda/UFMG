import HttpRestController from "../../infra/http/HttpRestController";
import Request from "../../infra/http/Request";
import Response from "../../infra/http/Response";
import CallService from "../../service/CallService";

export default class CreateNewCallController implements HttpRestController {

    private callService: CallService = new CallService();

    async execute(request: Request, response: Response): Promise<void> {
        try {
            const name = request.getBody().name;
            const phoneNumber = request.getBody().phoneNumber;
            const description = request.getBody().description;

            await this.callService.createNewCall(name, phoneNumber, description);

            response.status(200).end()
        } catch (erro: any) {
            response.status(400).send({
                message: erro.message
            });
        }
    }
}